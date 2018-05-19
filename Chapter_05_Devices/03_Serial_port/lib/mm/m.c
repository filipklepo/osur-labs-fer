#define _M_C_

#include <lib/m.h>
#include <types/bits.h>

#define IND_PUT(SIZE) ( SIZE < INCREMENT ? 0 : 1 + msb_index ( SIZE  ) - LOG2_INCR  )
#define IND_GET(SIZE) ( 1 + IND_PUT ( SIZE - 1 ) )

#ifndef ASSERT
#include ASSERT_H
#endif

static void m_print ( m_mpool_t *mpool, char *msg ) {
	int i, vel = 0;
	m_hdr_t *chunk;
	
	kprintf ( ">>>> %s\n", msg );
	for ( i = 0; i < N; i++ ) {
		chunk = mpool->first[i];
        if( ! chunk ) continue;
		kprintf ( "%d (%d): ", i, vel );
		while ( chunk ) {
			kprintf ( "adr:%x size:%d  ", chunk, GET_SIZE(chunk) );
			chunk = chunk->next;
		}
		kprintf ( "\n" );
		vel = ( i == 0 ? INCREMENT : vel * 2 );
	}
	kprintf ( "<<<< %s\n", msg );
}

/*!
 * Initialize dynamic memory manager
 * \param mem_segm Memory pool start address
 * \param size Memory pool size
 * \return memory pool descriptor
*/
void *m_init ( void *mem_segm, size_t size )
{
	size_t start, end;
	m_hdr_t *chunk, *border;
	m_mpool_t *mpool;

	ASSERT ( mem_segm && size > sizeof (m_hdr_t) * 2 );

	/* align all on 'size_t' (if already not aligned) */
	start = (size_t) mem_segm;
	end = start + size;
	ALIGN_FW ( start );
	mpool = (void *) start;		/* place mm descriptor here */
	start += sizeof (m_mpool_t);
	ALIGN ( end );
    if ( end - start < 2 * HEADER_SIZE ) return NULL;

    for(int i = 0; i < N; ++i) {
        mpool->first[i] = NULL;
    }

	border = (m_hdr_t *) start;
	border->size = sizeof (size_t);
	MARK_USED ( border );

	chunk = GET_AFTER ( border );
	chunk->size = end - start - 2 * sizeof(size_t);
	MARK_FREE ( chunk );
	CLONE_SIZE_TO_TAIL ( chunk );

	border = GET_AFTER ( chunk );
	border->size = sizeof (size_t);
	MARK_USED ( border );

	m_insert_chunk ( mpool, chunk ); /* first and only free chunk */

	return mpool;
}

/*!
 * Get free chunk with required size (or slightly bigger)
 * \param mpool Memory pool to be used 
 * \param size Requested chunk size
 * \return Block address, NULL if can't find adequate free chunk
 */
void *m_alloc ( m_mpool_t *mpool, size_t size )
{
    m_hdr_t *iter, *chunk;

	ASSERT ( mpool );

	size += sizeof (size_t) * 2; /* add header and tail size */
	if ( size < HEADER_SIZE )
		size = HEADER_SIZE;

	/* align request size to higher 'size_t' boundary */
	ALIGN_FW ( size );

    size_t mpool_index = IND_GET(size); 
    iter = NULL;
    while( iter == NULL && mpool_index < N ) iter = mpool->first[mpool_index++];

    if( iter == NULL )
        return NULL;

    m_print ( mpool, "prije dodjele");
	if ( iter->size > size + HEADER_SIZE )
	{
        m_remove_chunk( mpool, iter );
		/* split chunk */
		/* first part remains in free list, just update size */
		iter->size -= size;
		CLONE_SIZE_TO_TAIL ( iter );
        MARK_FREE ( iter );
        m_insert_chunk( mpool, iter );

		chunk = GET_AFTER ( iter );
		chunk->size = size;
	}
	else { /* give whole chunk */
		chunk = iter;

		/* remove it from free list */
		m_remove_chunk ( mpool, chunk );
	}

	MARK_USED ( chunk );

    m_print ( mpool, "nakon dodjele");
	return ( (void *) chunk ) + sizeof (size_t);
}

/*!
 * Free memory chunk
 * \param mpool Memory pool to be used (if NULL default pool is used)
 * \param chunk Chunk location (starting address)
 * \return 0 if successful, -1 otherwise
 */
int m_free ( m_mpool_t *mpool, void *chunk_to_be_freed )
{
    kprintf("$$$$$$ chunk_to_be_freed addr: %d\n", chunk_to_be_freed);
	m_hdr_t *chunk, *before, *after;

	ASSERT ( mpool && chunk_to_be_freed );

	chunk = chunk_to_be_freed - sizeof ( size_t );
    kprintf("$$$$$$ its size: %d\n", chunk->size);
	ASSERT ( CHECK_USED ( chunk ) );

	MARK_FREE ( chunk ); /* mark it as free */

	/* join with left? */
	before = ( (void *) chunk ) - sizeof ( size_t );
	if ( CHECK_FREE ( before ) )
	{
        before = GET_HDR ( before );
	    m_remove_chunk ( mpool, before );
        kprintf("!!!!!! Joining with before with size %d\n", before->size);
     	before->size += chunk->size; /* join */
	    chunk = before;
	}

	/* join with right? */
	after = GET_AFTER ( chunk );
	if ( CHECK_FREE ( after ) )
	{
        kprintf("!!!!!! Joining with after %d\n", after->size & 1);
		m_remove_chunk ( mpool, after );
		chunk->size += after->size; /* join */
	}

	/* insert chunk in free list */
	m_insert_chunk ( mpool, chunk );

	/* set chunk tail */
	CLONE_SIZE_TO_TAIL ( chunk );

    m_print ( mpool, "nakon oslobadjanja" );

	return 0;
}

/*!
 * Routine that removes an chunk from 'free' list (free_list)
 * \param mpool Memory pool to be used
 * \param chunk Chunk header
 */
static void m_remove_chunk ( m_mpool_t *mpool, m_hdr_t *chunk )
{
    size_t mpool_index = IND_PUT(chunk->size);

    if ( chunk == mpool->first[mpool_index] )
        mpool->first[mpool_index] = chunk->next;
    else
        chunk->prev->next = chunk->next;

    if ( chunk->next != NULL )
        chunk->next->prev = chunk->prev;
}

/*!
 * Routine that insert chunk into 'free' list (free_list)
 * \param mpool Memory pool to be used
 * \param chunk Chunk header
 */
static void m_insert_chunk ( m_mpool_t *mpool, m_hdr_t *chunk )
{
    chunk->prev = NULL;
    chunk->next = NULL;
    size_t mpool_index = IND_PUT(chunk->size);
    if ( ! mpool->first[mpool_index] )
    {
        mpool->first[mpool_index] = chunk;
    } else {
        m_hdr_t *last = mpool->first[mpool_index];
        while(last->next && (last = last->next));        
        last->next = chunk;
        chunk->prev = last;
    }
}

