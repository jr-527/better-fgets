#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "get_key.c"
#include "circular_buffer.c"
#include "enter_line.h"

int last_len = 0;
int this_len;
char up_buf[1024];

/*
#define update(...) do { \
    this_len = sprintf(up_buf, __VA_ARGS__); \
    printf("\r"); \
    for (int i = 0; i < last_len; i++) { \
        printf(" "); \
    } \
    printf("\r%s", up_buf); \
    last_len = this_len; \
    fflush(stdout); \
} while(0)
*/

#define update(...) do { \
    finish_update(sprintf(up_buf, __VA_ARGS__)); \
} while(0)

void finish_update(int this_len) {
    fprintf(stderr, "\r");
    for (int i = 0; i < last_len; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\r%s", up_buf);
    last_len = this_len;
    fflush(stderr);
}

struct LineHistNode {
    struct LineHistNode* prev;
    struct LineHistNode* next;
    char* line;
    int len;
};

struct LineHistNode* current = NULL;
struct LineHistNode* oldest = NULL;
struct LineHistNode* latest = NULL;
struct LineHistNode tmp_hist = {NULL, NULL, NULL, -1};
#define MAX_SAVED_LINES (20)
int num_hist_saved = 0;

// Go up a line
// Returns 0 if no such line exists, 1 otherwise
int prev_line() {
    if (current == oldest) {
        return 0;
    }
    if (current == NULL) {
        current = latest;
    } else {
        current = current->prev;
    }
    return 1;
}

// Read the current line to the two circular buffers
void read_hist_line(Buffer* left, Buffer* right) {
    if (current == NULL) {
        buffer_from_str(left, "");
    } else {
        buffer_from_str(left, current->line);
    }
    buffer_from_str(right, "");
}

// Go down a line. Return 0 if no such line exists, 1 otherwise.
int next_line() {
    if (current == NULL) {
        return 0;
    }
    current = current->next;
    return 1;
}

// Go to the latest line
void latest_line() {
    current = NULL;
}

// Makes the first history line by reading the buffers
void init_history(Buffer* left, Buffer* right, char out_buf[]) {
    int n = buffer_to_string(left, out_buf);
    n += buffer_to_string(right, out_buf+n);
    current = malloc(sizeof(*current));
    oldest = current;
    latest = current;
    current->len = n;
    current->line = malloc(n+1);
    strncpy(current->line, out_buf, n);
    current->line[n] = '\0';
    num_hist_saved = 1;
}

int at_last() {
    return (current == latest) || (current == NULL);
}

void add_to_history(Buffer* left, Buffer* right, char out_buf[]) {
    if (num_hist_saved == 0) {
        init_history(left, right, out_buf);
        return;
    }
    int n = buffer_to_string(left, out_buf);
    n += buffer_to_string(right, out_buf+n);
    if (n == 0) {
        current = latest;
        return;
    }
    if (!strcmp(out_buf, latest->line)) {
        // If line is same as previous line, do nothing.
        return;
    }
    if (num_hist_saved > 1 && !strcmp(out_buf, latest->prev->line)) {
        if (num_hist_saved == 2) {
            latest->prev = NULL;
            latest->next = oldest;
            oldest->prev = latest;
            oldest->next = NULL;
            latest = oldest;
            oldest = latest->prev;
        } else {
            struct LineHistNode* tmp = latest->prev;
            latest->prev = tmp->prev;
            tmp->prev->next = latest;
            latest->next = tmp;
            tmp->prev = latest;
            tmp->next = NULL;
        }
        return;
    }
    struct LineHistNode* out;
    if (num_hist_saved == MAX_SAVED_LINES) {
        // if we're full of lines, just re-use the oldest node
        out = oldest;
        oldest = oldest->next;
        out->next = NULL;
        oldest->prev = NULL;
        free(out->line);
    } else {
        out = malloc(sizeof(*out));
        num_hist_saved++;
    }
    out->prev = latest;
    latest->next = out;
    latest = out;
    out->line = malloc(n+1);
    strncpy(out->line, out_buf, n);
    out->line[n] = '\0';
    current = NULL;
    // current = out;
    // if line is same as previous line, do nothing
    // otherwise delete oldest line in history if length is too long,
    // make this one the newest
}

void set_tmp_hist(Buffer* left, Buffer* right, char out_buf[]) {
    int n = buffer_to_string(left, out_buf);
    n += buffer_to_string(right, out_buf+n);
    //if (n == 0) {
    //    return;
    //}
    tmp_hist.len = n;
    tmp_hist.line = malloc(n+1);
    tmp_hist.line[n] = '\0';
    strncpy(tmp_hist.line, out_buf, n);
}

int tmp_hist_exists() {
    return tmp_hist.len >= 0;
}

void reset_tmp_hist() {
    tmp_hist.len = -1;
    if (tmp_hist.line != NULL) {
        free(tmp_hist.line);
    }
}

void restore_tmp_hist(Buffer* left) {
    buffer_from_str(left, tmp_hist.line);
}

void print_history() {
    struct LineHistNode* cur = oldest;
    fprintf(stderr, "\nHistory (%d):\n", num_hist_saved);
    while (cur != NULL) {
        if (cur == current) {
            fprintf(stderr, ">> ");
        }
        fprintf(stderr, "entry: %s\n", cur->line);
        cur = cur->next;
    }

}

// For regular printable keys,
// ctrl+key returns key-96
// returns:
// 0 for normal return
// -1 for buffer full
// -2 for failing to parse a key
// -3 for interrupt
int type_line_helper(Buffer* left, Buffer* right, char out_buf[]) {
    buffer_from_str(left, "");
    buffer_from_str(right, "");
    reset_tmp_hist();
    int tmp_hist_available = 0;
    while (1) {
        key_T k = nextkey();
        if (k == -1) {
            fprintf(stderr, "\n");
            return -2;
        }
        if ((32 <= k) & (k <= 126)) { // regular character
            if (buffer_addright(left, (char)k) == -1) {
                fprintf(stderr, "\n");
                return -1;
            }
        } else if ((k == 127) || (k == 8)) { // backspace
            buffer_popright(left);
        } else if (k == 3) { // interrupt
            fprintf(stderr, "\n");
            return -3;
        } else if ((k == 4) || (k == 23)) { // eof
            fprintf(stderr, "\n");
            return 0;
        } else if ((k == 10) || (k == 13)) { // enter
            add_to_history(left, right, out_buf);
            latest_line();
            fprintf(stderr, "\n");
            return 0;
        } else if (k == 27) { // escape
            buffer_from_str(left, "");
            buffer_from_str(right, "");
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat-zero-length"
            update("");
            #pragma GCC diagnostic pop
            return 0;
        } else if (k < 0) { // non-ASCII key
            if (k == (key_T)LEFT) {
                if (left->count > 0) {
                    if (buffer_addleft(right, buffer_popright(left)) == -1) {
                        fprintf(stderr, "\n");
                        return -1;
                    }
                }
            } else if (k == (key_T)RIGHT) {
                if (right->count > 0) {
                    if (buffer_addright(left, buffer_popleft(right)) == -1) {
                        fprintf(stderr, "\n");
                        return -1;
                    }
                }
            } else if (k == (key_T)HOME) {
                while (left->count > 0) {
                    if (buffer_addleft(right, buffer_popright(left)) == -1) {
                        fprintf(stderr, "\n");
                        return -1;
                    }
                }
            } else if (k == (key_T)END) {
                while (right->count > 0) {
                    if (buffer_addright(left, buffer_popleft(right)) == -1) {
                        fprintf(stderr, "\n");
                        return -1;
                    }
                }
            } else if (k == (key_T)DEL) {
                buffer_popleft(right);
            } else if (k == (key_T)UP) {
                // printf("up\n");
                if (!prev_line()) {
                    continue;
                }
                if (at_last()) {// && prev_line()) {
                    tmp_hist_available = 1;
                    // printf("  at bottom\n  set_tmp_hist\n  read_hist_line\n");
                    // add_to_history(left, right, out_buf);
                    set_tmp_hist(left, right, out_buf);
                    read_hist_line(left, right);
                // } else if (prev_line()) {
                } else {
                    // printf("  else if prev_line went through\n");
                    read_hist_line(left, right);
                }
            } else if (k == (key_T)DOWN) {
                // printf("down, at_last: %d\n", at_last());
                if (!at_last()) {
                    // printf("  not at bottom\n");
                    if (next_line()) {
                        // printf("  next_line went through\n");
                        read_hist_line(left, right);
                    } else if (tmp_hist_available && tmp_hist_exists()) {
                        // printf("  tmp_hist_exists went through\n");
                        tmp_hist_available = 0;
                        restore_tmp_hist(left);
                        latest_line();
                    }
                } else if (tmp_hist_available && at_last() && tmp_hist_exists()) {
                    // printf("  restore_tmp_hist\n");
                    tmp_hist_available = 0;
                    restore_tmp_hist(left);
                    latest_line();
                }
            }
        } else {
            //printf(" unknown k: %d\n", k);
            fprintf(stderr, "\n");
            return -2;
        }
        int n = buffer_to_string(left, out_buf);
        buffer_to_string(right, out_buf+n);
        update("%s", out_buf);
        out_buf[n] = '\0';
        fprintf(stderr, "\r%s", out_buf);
        fflush(stderr);
    }
}

// returns:
// 0: success
// -1 for buffer full
// -2 for failing to parse a key
// -3 for interrupt
int type_line(char out[]) {
    char left_buf[511];
    Buffer left = create_buffer(left_buf, sizeof(left_buf));
    char right_buf[511];
    char out_buf[1024];
    Buffer right = create_buffer(right_buf, sizeof(right_buf));
    int return_code = type_line_helper(&left, &right, out_buf);
    if (return_code == -2) {
        //fprintf(stderr, "\nError: could not recognize key\n");
        return -2;
    } else if (return_code == -1) {
        //fprintf(stderr, "\nError: buffer full\n");
        return -1;
    }
    //fprintf(stderr, "\n");
    int n = buffer_to_string(&left, out);
    buffer_to_string(&right, out+n);
    return return_code;
}

#if __INCLUDE_LEVEL__ == 0
int main() {
    //struct pollfd pfd = { .fd=0, .events=POLLIN };
    //type_line(pfd);
    char b[1024];
    while (type_line(b) >= 0) {
        if (b[0] == 'q') {
            disable_quiet_input();
            return 0;
        }
        printf("type_line output: \"%s\"\n", b);
    }
    //printf("\n");
    return 0;
}
#endif
