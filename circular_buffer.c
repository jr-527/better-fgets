#include <stdio.h>
#include <string.h>

typedef struct Buffer {
    char* array;
    int capacity;
    int left; // using Python slicing convention
    int right;
    int count;
} Buffer;

Buffer create_buffer(char* array, int length) {
    Buffer out;
    out.array = array;
    out.capacity = length;
    out.left = 0;
    out.right = 0;
    out.count = 0;
    return out;
}

int buffer_addright(Buffer* b, char x) {
    if (b->count == b->capacity) {
        return -1;
    }
    b->count++;
    int new_right = b->right+1;
    if (new_right == b->capacity) {
        new_right = 0;
    }
    b->array[b->right] = x;
    b->right = new_right;
    return 0;
}

// returns '\0' if buffer empty
char buffer_peekleft(Buffer* b) {
    if (b->count == 0) {
        return '\0';
    }
    return b->array[b->left];
}

char buffer_popleft(Buffer* b) {
    if (b->count == 0) {
        return '\0';
    }
    char out = b->array[b->left];
    int idx = b->left + 1;
    if (idx == b->capacity) {
        idx = 0;
    }
    b->left = idx;
    b->count -= 1;
    return out;
}

char buffer_peekright(Buffer* b) {
    if (b->count == 0) {
        return '\0';
    }
    int idx = b->right-1;
    if (idx < 0) {
        return b->array[b->capacity-1];
    }
    return b->array[idx];
}

char buffer_popright(Buffer* b) {
    if (b->count == 0) {
        return '\0';
    }
    int idx = b->right-1;
    if (idx < 0) {
        idx = b->capacity-1;
    }
    b->right = idx;
    b->count -= 1;
    return b->array[idx];
}

int buffer_addleft(Buffer* b, char x) {
    if (b->count == b->capacity || x == '\0') {
        return -1;
    }
    b->count++;
    int new_left = (b->left - 1);
    if (new_left == -1) {
        new_left = b->capacity-1;
    }
    b->array[new_left] = x;
    b->left = new_left;
    return 0;
}

// out must have length greater than b->count
int buffer_to_string(Buffer* b, char* out) {
    int stop1 = b->right;
    int stop2 = -1;
    if (b->right < b->left) {
        stop2 = stop1;
        stop1 = b->capacity;
    }
    int j = 0;
    for (int i = b->left; i < stop1; i++) {
        out[j++] = b->array[i];
    }
    for (int i = 0; i < stop2; i++) {
        out[j++] = b->array[i];
    }
    out[j] = '\0';
    return j;
}

int buffer_from_str(Buffer* b, char str[]) {
    b->left = 0;
    b->count = snprintf(b->array, b->capacity, "%s", str);
    b->right = b->count;
    return 0;
}

/*
int main() {
    char x[64];
    char y[65];
    Buffer b = create_buffer(x, 64);
    buffer_addright(&b, 'b');
    buffer_addright(&b, 'c');
    buffer_addright(&b, 'd');
    printf("popped: %c\n", buffer_popright(&b));
    buffer_addleft(&b, 'a');
    buffer_to_string(&b, y);
    printf("%s\n", y);
    return 0;
}
*/
