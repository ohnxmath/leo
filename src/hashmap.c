#include "hashmap.h"

/* CRC8 */
#define CRC8_TOPBIT (1 << 7)
#define CRC8_POLYNOMIAL 0x9b

static unsigned char table8[256];
static int crc8_hinit = 0;

void crc8_init() {
    int dividend;
    unsigned char bit, remainder;
    
    crc8_hinit = 1;
    
    /* Compute the remainder of each possible dividend. */
    for (dividend = 0; dividend < 256; dividend++) {
        /* Start with the dividend followed by zeros. */
        remainder = dividend;

        /* Perform modulo-2 division, a bit at a time. */
        for (bit = 8; bit > 0; --bit) {
            /* Try to divide the current data bit. */
            if (remainder & CRC8_TOPBIT) {
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }

        /* Store the result into the table. */
        table8[dividend] = remainder;
    }
}

unsigned short crc8_block(const unsigned char *bytes, long byteLen) {
    long rpos = 0;
    unsigned char remainder = 0, data;
    
    if (!crc8_hinit) crc8_init();
    
    /* Divide the message by the polynomial, a byte at a time. */
    for (rpos = 0; rpos < byteLen; ++rpos) {
        data = bytes[rpos] ^ remainder;
        remainder = table8[data] ^ (remainder << 8);
    }

    /* The final remainder is the CRC. */
    return remainder & 0xff;
}

/* CRC16 */
static const unsigned short table16[256] = {
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x0919, 0x1890, 0x2A0B, 0x3B82, 0x4F3D, 0x5EB4, 0x6C2F, 0x7DA6,
    0x8551, 0x94D8, 0xA643, 0xB7CA, 0xC375, 0xD2FC, 0xE067, 0xF1EE,
    0x1232, 0x03BB, 0x3120, 0x20A9, 0x5416, 0x459F, 0x7704, 0x668D,
    0x9E7A, 0x8FF3, 0xBD68, 0xACE1, 0xD85E, 0xC9D7, 0xFB4C, 0xEAC5,
    0x1B2B, 0x0AA2, 0x3839, 0x29B0, 0x5D0F, 0x4C86, 0x7E1D, 0x6F94,
    0x9763, 0x86EA, 0xB471, 0xA5F8, 0xD147, 0xC0CE, 0xF255, 0xE3DC,
    0x2464, 0x35ED, 0x0776, 0x16FF, 0x6240, 0x73C9, 0x4152, 0x50DB,
    0xA82C, 0xB9A5, 0x8B3E, 0x9AB7, 0xEE08, 0xFF81, 0xCD1A, 0xDC93,
    0x2D7D, 0x3CF4, 0x0E6F, 0x1FE6, 0x6B59, 0x7AD0, 0x484B, 0x59C2,
    0xA135, 0xB0BC, 0x8227, 0x93AE, 0xE711, 0xF698, 0xC403, 0xD58A,
    0x3656, 0x27DF, 0x1544, 0x04CD, 0x7072, 0x61FB, 0x5360, 0x42E9,
    0xBA1E, 0xAB97, 0x990C, 0x8885, 0xFC3A, 0xEDB3, 0xDF28, 0xCEA1,
    0x3F4F, 0x2EC6, 0x1C5D, 0x0DD4, 0x796B, 0x68E2, 0x5A79, 0x4BF0,
    0xB307, 0xA28E, 0x9015, 0x819C, 0xF523, 0xE4AA, 0xD631, 0xC7B8,
    0x48C8, 0x5941, 0x6BDA, 0x7A53, 0x0EEC, 0x1F65, 0x2DFE, 0x3C77,
    0xC480, 0xD509, 0xE792, 0xF61B, 0x82A4, 0x932D, 0xA1B6, 0xB03F,
    0x41D1, 0x5058, 0x62C3, 0x734A, 0x07F5, 0x167C, 0x24E7, 0x356E,
    0xCD99, 0xDC10, 0xEE8B, 0xFF02, 0x8BBD, 0x9A34, 0xA8AF, 0xB926,
    0x5AFA, 0x4B73, 0x79E8, 0x6861, 0x1CDE, 0x0D57, 0x3FCC, 0x2E45,
    0xD6B2, 0xC73B, 0xF5A0, 0xE429, 0x9096, 0x811F, 0xB384, 0xA20D,
    0x53E3, 0x426A, 0x70F1, 0x6178, 0x15C7, 0x044E, 0x36D5, 0x275C,
    0xDFAB, 0xCE22, 0xFCB9, 0xED30, 0x998F, 0x8806, 0xBA9D, 0xAB14,
    0x6CAC, 0x7D25, 0x4FBE, 0x5E37, 0x2A88, 0x3B01, 0x099A, 0x1813,
    0xE0E4, 0xF16D, 0xC3F6, 0xD27F, 0xA6C0, 0xB749, 0x85D2, 0x945B,
    0x65B5, 0x743C, 0x46A7, 0x572E, 0x2391, 0x3218, 0x0083, 0x110A,
    0xE9FD, 0xF874, 0xCAEF, 0xDB66, 0xAFD9, 0xBE50, 0x8CCB, 0x9D42,
    0x7E9E, 0x6F17, 0x5D8C, 0x4C05, 0x38BA, 0x2933, 0x1BA8, 0x0A21,
    0xF2D6, 0xE35F, 0xD1C4, 0xC04D, 0xB4F2, 0xA57B, 0x97E0, 0x8669,
    0x7787, 0x660E, 0x5495, 0x451C, 0x31A3, 0x202A, 0x12B1, 0x0338,
    0xFBCF, 0xEA46, 0xD8DD, 0xC954, 0xBDEB, 0xAC62, 0x9EF9, 0x8F70
};

unsigned short crc16_block(const unsigned char *bytes, long byteLen) {
    unsigned char remainder = 0;
    
    while (byteLen--)
        remainder = (remainder << 8) ^ table16[((remainder >> 8) ^ *bytes++)];

    /* The final remainder is the CRC. */
    return remainder;
}

/* Hashmap */
hashmap *hashmap_new() {
    hashmap *ret;
    hashmap_entry_list *buckets;

    if (!crc8_hinit) crc8_init();

    /* clear initial values */
    ret = calloc(1, sizeof(hashmap));
    buckets = calloc(256, sizeof(hashmap_entry));
    
    if (ret == NULL || buckets == NULL) return NULL;

    ret->hash_func = crc8_block;
    ret->num_entry = 0;
    ret->bucketsize = 256;
    ret->buckets = buckets;
    return ret;
}

void hashmap_resize(hashmap *in) {
    hashmap_entry_list *bucketsold, *hel;
    hashmap_entry *he;
    int i, j;
    
    if (in->bucketsize > 256) return;
    
    bucketsold = in->buckets;
    in->bucketsize = 65536;
    in->hash_func = crc16_block;
    in->buckets = calloc(65536, sizeof(hashmap_entry));
    
    /* loop through all the buckets to copy over */
    for (i = 0; i < 256; i++) {
        hel = &bucketsold[i];

        /* loop through all the key/value pairs */
        for (j = 0; j < hel->vlen; j++) {
            he = &hel->values[j];
            
            /* call the function */
            hashmap_put(in, he->key, he->value);
        }
    }
    free(bucketsold);
    
    /* at this point, everything has been copied over */
}

void hashmap_put(hashmap *in, const char *key, void *value) {
    unsigned short hash;
    hashmap_entry_list *hel;
    hashmap_entry *he;
    
    /* get hash value */
    hash = in->hash_func((const unsigned char *)key, (long)strlen(key));
    hel = &in->buckets[hash];
    
    /* reallocate memory if necessary */
    if (hel->vroom < ++(hel->vlen)) {
        hel->vroom = (hel->vroom == 0 ? 4 : hel->vroom*2);
        he = realloc(hel->values, hel->vroom * sizeof(hashmap_entry));

        if (he == NULL) {
            return; /* TODO: Throw exception */
        }
        
        hel->values = he;
    }
    
    /* jump to end of hashmap entry list */
    he = &hel->values[hel->vlen-1];
    
    he->key = strdup(key);
    he->value = value;
    
    /* success */
    in->num_entry++;
}

void hashmap_remove(hashmap *in, const char *key) {
    unsigned short hash;
    hashmap_entry_list *hel;
    hashmap_entry *he;
    int i;

    /* get hash value */
    hash = crc8_block((const unsigned char *)key, (long)strlen(key));
    hel = &in->buckets[hash];

    /* loop through all the key/value pairs with that hash */
    for (i = 0; i < hel->vlen; i++) {
        he = &hel->values[i];
        if (!strcmp(key, he->key)) goto found;
    }
    /* no such key */
    return;

    /* success */
    found:
    /* free the key */
    free(he->key);
    /* copy over the values past this to over this */
    memcpy(hel->values + i, hel->values + i + 1, sizeof(hashmap_entry) * (hel->vlen - i));
    /* decrease the length */
    hel->vlen--;
}

void *hashmap_get(hashmap *in, const char *key) {
    unsigned short hash;
    hashmap_entry_list *hel;
    hashmap_entry *he;
    int i;
    
    /* get hash value */
    hash = in->hash_func((const unsigned char *)key, (long)strlen(key));
    hel = &in->buckets[hash];
    
    /* loop through all the key/value pairs with that hash */
    for (i = 0; i < hel->vlen; i++) {
        he = &hel->values[i];
        if (!strcmp(key, he->key)) goto found; /* found exact key */
    }
    return NULL;
    
    /* success */
    found:
    return he->value;
}

int hashmap_iterate(hashmap *in, hashmap_iterator iter, void *context) {
    hashmap_entry_list *hel;
    hashmap_entry *he;
    int i, j;
    
    /* loop through all the buckets */
    for (i = 0; i < in->bucketsize; i++) {
        hel = &in->buckets[i];

        /* loop through all the key/value pairs */
        for (j = 0; j < hel->vlen; j++) {
            he = &hel->values[j];
            
            /* call the function */
            if (iter(context, he->key, he->value))
                return -1;
        }
    }
    
    return 0;
}

void hashmap_empty(hashmap *in) {
    hashmap_entry_list *hel;
    hashmap_entry *he;
    int i, j;
    
    /* loop through all the buckets */
    for (i = 0; i < in->bucketsize; i++) {
        hel = &in->buckets[i];
        
        /* loop through all the key/value pairs */
        for (j = 0; j < hel->vlen; j++) {
            he = &hel->values[j];
            
            /* free the key */
            free(he->key);
        }
        
        /* reset the key/value pairs array */
        free(hel->values);
        hel->values = NULL;
        hel->vlen = 0;
        hel->vroom = 0;
    }
}

void hashmap_destroy(hashmap *in) {
    /* empty hashmap */
    hashmap_empty(in);
    
    /* clean up bucket storage */
    free(in->buckets);
    free(in);
}
