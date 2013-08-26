/*
 * SHL - HTable Tests
 *
 * Copyright (c) 2012-2013 David Herrmann <dh.herrmann@gmail.com>
 * Dedicated to the Public Domain.
 */

#include "test_common.h"

static struct shl_htable ht = SHL_HTABLE_INIT_STR(ht);
static struct shl_htable uht = SHL_HTABLE_INIT_ULONG(uht);

struct node {
	char huge_padding[16384];
	uint8_t v;
	char paaaaaadding[16384];
	char *key;
	unsigned long ul;
	char more_padding[32768];
	size_t hash;
};

#define to_node(_key) shl_hlist_offsetof((_key), struct node, key)
#define ul_to_node(_key) shl_hlist_offsetof((_key), struct node, ul)

static struct node o[] = {
	{ .v = 0, .key = "o0", .ul = 0 },
	{ .v = 1, .key = "o1", .ul = 1 },
	{ .v = 2, .key = "o2", .ul = 2 },
	{ .v = 3, .key = "o3", .ul = 3 },
	{ .v = 4, .key = "o4", .ul = 4 },
	{ .v = 5, .key = "o5", .ul = 5 },
	{ .v = 6, .key = "o6", .ul = 6 },
	{ .v = 7, .key = "o7", .ul = 7 },
};

START_TEST(test_htable_str)
{
	int r, i;
	char **k;
	bool b;

	/* insert once, remove once, try removing again */

	ck_assert(!o[0].hash);
	r = shl_htable_insert_str(&ht, &o[0].key, &o[0].hash);
	ck_assert(!r);
	ck_assert(o[0].hash == shl_htable_rehash_str(&o[0].key, NULL));

	b = shl_htable_remove_str(&ht, o[0].key, &o[0].hash, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(to_node(k)->v == 0);

	k = NULL;
	b = shl_htable_remove_str(&ht, o[0].key, &o[0].hash, &k);
	ck_assert(!b);
	ck_assert(k == NULL);

	/* insert twice, remove twice, try removing again */

	r = shl_htable_insert_str(&ht, &o[0].key, &o[0].hash);
	ck_assert(!r);
	ck_assert(o[0].hash == shl_htable_rehash_str(&o[0].key, NULL));

	r = shl_htable_insert_str(&ht, &o[0].key, &o[0].hash);
	ck_assert(!r);
	ck_assert(o[0].hash == shl_htable_rehash_str(&o[0].key, NULL));

	b = shl_htable_remove_str(&ht, o[0].key, &o[0].hash, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(to_node(k)->v == 0);

	b = shl_htable_remove_str(&ht, o[0].key, &o[0].hash, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(to_node(k)->v == 0);

	k = NULL;
	b = shl_htable_remove_str(&ht, o[0].key, &o[0].hash, &k);
	ck_assert(!b);
	ck_assert(k == NULL);

	/* same as before but without hash-cache */

	r = shl_htable_insert_str(&ht, &o[0].key, NULL);
	ck_assert(!r);

	r = shl_htable_insert_str(&ht, &o[0].key, NULL);
	ck_assert(!r);

	b = shl_htable_remove_str(&ht, o[0].key, NULL, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(to_node(k)->v == 0);

	b = shl_htable_remove_str(&ht, o[0].key, NULL, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(to_node(k)->v == 0);

	k = NULL;
	b = shl_htable_remove_str(&ht, o[0].key, NULL, &k);
	ck_assert(!b);
	ck_assert(k == NULL);

	/* insert all elements and verify empty hash-caches */

	o[0].hash = 0;
	for (i = 0; i < 8; ++i) {
		ck_assert(!o[i].hash);
		r = shl_htable_insert_str(&ht, &o[i].key, &o[i].hash);
		ck_assert(!r);
		ck_assert(o[i].hash == shl_htable_rehash_str(&o[i].key, NULL));
	}

	/* verify */

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_lookup_str(&ht, o[i].key, NULL, &k);
		ck_assert(b);
		ck_assert(k != NULL);
		ck_assert(to_node(k)->v == i);
	}

	/* remove all elements again */

	for (i = 0; i < 8; ++i) {
		b = shl_htable_remove_str(&ht, o[i].key, NULL, &k);
		ck_assert(b);
		ck_assert(k != NULL);
		ck_assert(to_node(k)->v == i);
	}

	/* verify they're gone */

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_remove_str(&ht, o[i].key, NULL, &k);
		ck_assert(!b);
		ck_assert(k == NULL);
	}

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_lookup_str(&ht, o[i].key, NULL, &k);
		ck_assert(!b);
		ck_assert(k == NULL);
	}
}
END_TEST

START_TEST(test_htable_ulong)
{
	int r, i;
	unsigned long *k;
	bool b;

	/* insert once, remove once, try removing again */

	r = shl_htable_insert_ulong(&uht, &o[0].ul);
	ck_assert(!r);
	ck_assert(o[0].ul == shl_htable_rehash_ulong(&o[0].ul, NULL));

	b = shl_htable_remove_ulong(&uht, o[0].ul, &k);
	ck_assert(b);
	ck_assert(k != NULL);
	ck_assert(ul_to_node(k)->v == 0);

	k = NULL;
	b = shl_htable_remove_ulong(&uht, o[0].ul, &k);
	ck_assert(!b);
	ck_assert(k == NULL);

	/* insert all */

	for (i = 0; i < 8; ++i) {
		r = shl_htable_insert_ulong(&uht, &o[i].ul);
		ck_assert(!r);
	}

	/* verify */

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_lookup_ulong(&uht, o[i].ul, &k);
		ck_assert(b);
		ck_assert(k != NULL);
	}

	/* remove all elements again */

	for (i = 0; i < 8; ++i) {
		b = shl_htable_remove_ulong(&uht, o[i].ul, &k);
		ck_assert(b);
		ck_assert(k != NULL);
		ck_assert(ul_to_node(k)->v == i);
	}

	/* verify they're gone */

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_remove_ulong(&uht, o[i].ul, &k);
		ck_assert(!b);
		ck_assert(k == NULL);
	}

	for (i = 0; i < 8; ++i) {
		k = NULL;
		b = shl_htable_lookup_ulong(&uht, o[i].ul, &k);
		ck_assert(!b);
		ck_assert(k == NULL);
	}
}
END_TEST

TEST_DEFINE_CASE(misc)
	TEST(test_htable_str)
	TEST(test_htable_ulong)
TEST_END_CASE

TEST_DEFINE(
	TEST_SUITE(htable,
		TEST_CASE(misc),
		TEST_END
	)
)
