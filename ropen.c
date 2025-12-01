/*
 * ropen.c  –  Rift Stage-3 Hex Encoder (MinGW-safe)
 * Builds:  ropen.dll  +  ropen.exe  (CLI)
 * 2→1 sparse duplex, Red-Black AVL pruning, ELF-bootstep
 */

#ifdef _WIN32
#  define RIFT_API __declspec(dllexport)
#  ifdef RIFT_OPEN_MAIN
     /* CLI needs a main – no DLL decoration */
#  else
#    define RIFT_API __declspec(dllexport)
#  endif
#else
#  define RIFT_API __attribute__((visibility("default")))
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* ---------- Rift Triplet ---------- */
typedef struct {
    uint8_t  type;      /* 0 = hex, 1 = conjugate, 2 = epsilon */
    uint8_t  polarity;  /* '+' 0x2B  '-' 0x2D */
    uint16_t reserved;
} RIFTToken;

typedef struct {
    union { uint8_t u8; uint16_t u16; uint32_t u32; } value;
} RIFTTokenValue;

typedef struct {
    size_t size; void *ptr;
} RIFTTokenMemory;

/* ---------- Red-Black AVL ---------- */
typedef struct RBNode {
    uint32_t      key;
    uint8_t       val;
    uint8_t       polarity;
    float         confidence;
    uint8_t       color;
    uint8_t       height;
    struct RBNode *left, *right, *parent;
} RBNode;

static RBNode *rb_root = NULL;
static const float PRUNE_THRESHOLD = 0.5f;
static const int   PRUNE_STREAK    = 1;
static const uint8_t POLARITY_POS  = '+';
static const uint8_t POLARITY_NEG  = '-';

/* ---- utilities ---- */
static uint8_t height(RBNode *n) { return n ? n->height : 0; }
static int bf(RBNode *n) { return n ? (int)height(n->left) - (int)height(n->right) : 0; }
static void update_height(RBNode *n) { if (n) n->height = 1 + (height(n->left) > height(n->right) ? height(n->left) : height(n->right)); }

static RBNode *rotate_left(RBNode *x) {
    RBNode *y = x->right; if (!y) return x;
    x->right = y->left;  if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
    } else rb_root = y;
    y->left = x; x->parent = y;
    update_height(x); update_height(y);
    return y;
}

static RBNode *rotate_right(RBNode *x) {
    RBNode *y = x->left; if (!y) return x;
    x->left = y->right; if (y->right) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
    } else rb_root = y;
    y->right = x; x->parent = y;
    update_height(x); update_height(y);
    return y;
}

static void rebalance_up(RBNode *n) {
    while (n) {
        update_height(n);
        int b = bf(n);
        if (b > 1) {                       /* left heavy */
            if (bf(n->left) < 0) n->left = rotate_left(n->left);
            n = rotate_right(n);
        } else if (b < -1) {               /* right heavy */
            if (bf(n->right) > 0) n->right = rotate_right(n->right);
            n = rotate_left(n);
        } else n = n->parent;
    }
}

/* ---- insert / mark / prune ---- */
static RBNode *bst_insert(RBNode *root, RBNode *node) {
    if (!root) return node;
    if (node->key < root->key) {
        root->left = bst_insert(root->left, node);
        root->left->parent = root;
    } else if (node->key > root->key) {
        root->right = bst_insert(root->right, node);
        root->right->parent = root;
    } else { /* update in place */
        root->val        = node->val;
        root->confidence = node->confidence;
        root->polarity   = node->polarity;
        free(node);
        return root;
    }
    update_height(root);
    return root;
}

static void rb_insert(uint32_t key, uint8_t val, float conf, uint8_t pol) {
    RBNode *n = calloc(1, sizeof(*n));
    n->key = key; n->val = val; n->confidence = conf; n->polarity = pol;
    n->color = (rb_root ? 1 : 0);   /* root is BLACK */
    rb_root = bst_insert(rb_root, n);
    rebalance_up(n->parent ? n->parent : n);
}

static RBNode *find(uint32_t key) {
    RBNode *cur = rb_root;
    while (cur) {
        if (key == cur->key) return cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return NULL;
}

/* FIXED: streak[] is now file-static, not inside if-block */
static int streak[256] = {0};

static void mark_measurement(uint32_t key, float conf, uint8_t pol) {
    RBNode *n = find(key);
    if (!n) return;
    n->confidence = conf;
    if (pol) n->polarity = pol;
    /* pruning decision */
    if (conf < PRUNE_THRESHOLD || n->polarity == POLARITY_NEG) {
        streak[key & 0xFF]++;
        if (streak[key & 0xFF] >= PRUNE_STREAK) {
            n->val = 0; n->confidence = 0.0f;
        }
    } else streak[key & 0xFF] = 0;
}

/* ---------- 2→1 Sparse Duplex Encoding ---------- */
static uint8_t conjugate(uint8_t x) { return 0xF ^ x; }

static size_t rift_encode(const uint8_t *in, size_t in_len, uint8_t *out, bool polarity_A) {
    size_t out_len = 0;
    for (size_t i = 0; i < in_len; i += 2) {
        uint8_t a = in[i];
        uint8_t b = (i + 1 < in_len) ? in[i + 1] : 0x00; /* epsilon pad */
        uint8_t logical = (polarity_A ? a : conjugate(a)) ^ (polarity_A ? conjugate(b) : b);
        out[out_len++] = logical;
        rb_insert((uint32_t)out_len, logical, 1.0f, polarity_A ? POLARITY_POS : POLARITY_NEG);
    }
    return out_len;
}

/* ---------- Public API ---------- */
RIFT_API size_t rift_open(const char *path, uint8_t *out, size_t out_cap, bool polarity_A) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    uint8_t buf[4096];
    size_t total = 0;
    while (!feof(f) && total < out_cap) {
        size_t n = fread(buf, 1, sizeof(buf), f);
        size_t m = rift_encode(buf, n, out + total, polarity_A);
        total += m;
    }
    fclose(f);
    return total;
}

RIFT_API void rift_prune_negative(void) { /* already lazy */ }

/* ---------- CLI stub ---------- */
#ifdef RIFT_OPEN_MAIN
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <file> [A|B]\n", argv[0]); return 1; }
    bool A = (argc < 3 || argv[2][0] == 'A');
    uint8_t out[1 << 20];
    size_t n = rift_open(argv[1], out, sizeof(out), A);
    printf("Encoded %zu bytes (polarity %c)\n", n, A ? 'A' : 'B');
    for (size_t i = 0; i < n && i < 64; ++i) printf("%02X ", out[i]);
    puts("");
    return 0;
}
#endif
