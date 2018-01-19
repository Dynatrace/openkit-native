#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <gtest/gtest.h>

#include "threading_utils_atomic.h"
#include "threading_utils_thread.h"

class AtomicTest : public testing::Test
{
public:
	void SetUp() 
	{
		test_atomic = init_atomic(0);
	}

	void TearDown() 
	{
		destroy_atomic(test_atomic);
	}

	atomic_int32* get_atomic()
	{
		return test_atomic;
	}
private:
	atomic_int32 * test_atomic;
};
struct thread_info
{
	uint32_t thread_id;
};

void* incrementThread(void* arg)
{
	atomic_int32* atom = (atomic_int32*)arg;
	int32_t i;
	for (i = 0; i < 20; i++)
	{
		atomic_increment_and_get(atom);

		threading_sleep(1);
	}

	return NULL;
}

TEST_F(AtomicTest, IntegerIs100After5ThreadsEachDoing20Requests)
{
	atomic_int32* atom = get_atomic();

	ASSERT_TRUE(NULL != atom);

	threading_thread* t1 = create_thread(&incrementThread, atom);
	threading_thread* t2 = create_thread(&incrementThread, atom);
	threading_thread* t3 = create_thread(&incrementThread, atom);
	threading_thread* t4 = create_thread(&incrementThread, atom);
	threading_thread* t5 = create_thread(&incrementThread, atom);

	ASSERT_TRUE(t1 != NULL);
	ASSERT_TRUE(t2 != NULL);
	ASSERT_TRUE(t3 != NULL);
	ASSERT_TRUE(t4 != NULL);
	ASSERT_TRUE(t5 != NULL);

	if (t1 != NULL && t2 != NULL && t3 != NULL && t4 != NULL && t5 != NULL)
	{

		threading_thread_join(t1);
		threading_thread_join(t2);
		threading_thread_join(t3);
		threading_thread_join(t4);
		threading_thread_join(t5);

		destroy_thread(t1);
		destroy_thread(t2);
		destroy_thread(t3);
		destroy_thread(t4);
		destroy_thread(t5);
	}

	ASSERT_EQ(100, atom->integer_value);
}
