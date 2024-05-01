#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    LIT,
    CAT, // :3
    OR,
    END
} Tag;

typedef struct Ast Ast;

typedef union {
    char ch;
    Ast *children;
} Data;

struct Ast {
    Tag tag;
    Data data;
};

Ast create_lit(char c) {
    return (Ast){
        .tag = LIT,
        .data = 
            (Data){.ch = c}
        };
}

Ast create_seq(Ast* c, Tag tag) {
    return (Ast){
        .tag = tag,
        .data = 
            (Data){.children = c}
        };
}

void cleanup(Ast ast) {
    if (ast.tag != CAT || ast.tag != OR) return;
    
    Ast* children = ast.data.children;

    for (int i=0;children[i].tag != END; i++) {
        cleanup(children[i]);
    }

    free(ast.data.children);
}

void init(Ast *seq, int len, int cap) {
    Ast end = {END, (Data){ .children =  NULL}};
    for (int i=len; i < cap; i++) seq[i]=end;
}

void* expect_nonnull(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Oh no, it looks like I got too hungry for yo computer... *smacks lips*\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int ensure_nuf_space(void** vec, int required_cap, int cap, int size) {
    while (required_cap >= cap) {
        cap*=2;
    }
    *vec = expect_nonnull(realloc(*vec, cap*size));
    return cap;
}

int parse(char*, Ast*);

int parse_next(char* input, Ast* ret) {
    if (input[0] == '(') {
        int c=0, i=1;
        for(; input[i] != ')' || c != 0; i++) {
            switch (input[i]) {
                case '(':
                    c++;
                    break;
                case ')':
                    c--;
                    break;
                case '\\':
                    i++;
                    break;
            }
        }

        input[i]=0;
        parse(&input[1], ret);
        input[i]=')';
        return i+1;
    }
    int offset = input[0]=='\\';
    *ret = create_lit(input[offset]);
    return offset + 1;
}

int parse(char* input, Ast* ret) {
    Ast end = create_seq(NULL, END);

    int len = 0, cap = 10, len2 = 0, cap2 = 10;
    Ast* seq = expect_nonnull(malloc(sizeof(Ast)*cap));

    Ast* ptr = expect_nonnull(malloc(sizeof(Ast)*cap2));
    seq[len++] = create_seq(ptr, CAT);

    int i=0;

    while (input[i]) {
        if (input[i] == '|') {
            seq[len-1].data.children[len2] = end;
            cap = ensure_nuf_space(&seq, len+2, cap, sizeof(Ast));
            i++;
            len2=0;
            cap2=10;
            Ast* ptr = expect_nonnull(malloc(sizeof(Ast)*cap2));
            seq[len++] = create_seq(ptr, CAT);
            continue;
        }
        
        Ast** currp = &seq[len-1].data.children;
        
        if (input[i] == '*') {
            cleanup((*currp)[--len2]);
            (*currp)[len2]=end;
            i++;
            continue;
        }

        cap2 = ensure_nuf_space(currp, len2+2, cap2, sizeof(Ast));

        Ast* curr = *currp;

        if (input[i] == '[') {
            if (input[i+2]=='*') {
                i+=3;
                continue;
            }
            int c=0;
            while((input[i] != '|' || c != 0) && input[i] != 0) {
                switch (input[i]) {
                    case '(':
                        c++;
                        break;
                    case ')':
                        c--;
                        break;
                    case '\\':
                        i++;
                }
                i++;
            }
            if(input[i]==0) {
                seq[--len] = end;
                *ret = len == 1 ? seq[0] : create_seq(seq, OR);
                if(len == 1) free(seq);
                return i;
            }
            len2=0;
            i++;
            continue;
        }

        i += parse_next(&input[i], &curr[len2++]);
    }

    seq[len-1].data.children[len2]=end;
    seq[len] = end;

    *ret = len == 1 ? seq[0] : create_seq(seq, OR);

    return i;
}

char* create_example(Ast ast) {
    switch (ast.tag) {
        case LIT:
        {
            char* a = expect_nonnull(malloc(sizeof(char)*2));
            a[0] = ast.data.ch;
            a[1] = 0;
            return a;
        }
        case OR:
        {
            Ast* children = ast.data.children;

            for (int i = 0; children[i].tag != END; i++) {
                char* segment = create_example(children[i]);
                if (segment != NULL) return segment;
            }

            return NULL;
        }
        case CAT:
        {
            int len = 0, cap = 10;
            char* res = expect_nonnull(malloc(sizeof(char)*cap));
            
            Ast* children = ast.data.children;

            for (int i = 0; children[i].tag != END; i++) {
                char* segment = create_example(children[i]);

                if (segment == NULL) {
                    free(res);
                    return NULL;
                }
                
                int l = strlen(segment);
                cap = ensure_nuf_space(&res, len+l+1, cap, sizeof(char));
                strcpy(&res[len], segment);
                len+=l;
            }

            res[len] = 0;
            return res;
        }
        case END: {}
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int cap=8, len=0;
    char* input=expect_nonnull(malloc(sizeof(char)*cap));

    while (1) {
        printf(">> ");
        fflush(stdout);

        char c;
        while ((c=getchar()) != '\n') {
            if (c==EOF) {
                free(input);
                return EXIT_SUCCESS;
            }
            cap = ensure_nuf_space(&input, len+2, cap, sizeof(char));
            input[len++]=c;
        }

        input[len]=0;

        Ast ast;

        parse(input, &ast);

        char* ex = create_example(ast);

        if (ex != NULL) {
            printf("%s\n", ex);
            free(ex);
        } else printf(":(\n");

        cleanup(ast);
        fflush(stdout);
    }
}
