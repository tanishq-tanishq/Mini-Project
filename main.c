#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 30
#define COLS 80
#define MAX_OBJ 64

/* ── Canvas ── */
char canvas[ROWS][COLS + 1];

void canvas_clear() {
    for (int r = 0; r < ROWS; r++) {
        memset(canvas[r], ' ', COLS);
        canvas[r][COLS] = '\0';
    }
}

void plot(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

void display() {
    printf("\n+");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
    for (int r = 0; r < ROWS; r++)
        printf("|%s|\n", canvas[r]);
    printf("+");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
}

/* ── Drawing functions ── */
void draw_line(int r0, int c0, int r1, int c1, char ch) {
    int dr = abs(r1-r0), dc = abs(c1-c0);
    int sr = r0<r1 ? 1:-1, sc = c0<c1 ? 1:-1;
    int err = dr - dc;
    while (1) {
        plot(r0, c0, ch);
        if (r0==r1 && c0==c1) break;
        int e2 = 2*err;
        if (e2 > -dc) { err -= dc; r0 += sr; }
        if (e2 <  dr) { err += dr; c0 += sc; }
    }
}

void draw_rectangle(int r0, int c0, int r1, int c1, char ch) {
    for (int c = c0; c <= c1; c++) { plot(r0,c,ch); plot(r1,c,ch); }
    for (int r = r0; r <= r1; r++) { plot(r,c0,ch); plot(r,c1,ch); }
}

void draw_circle(int cr, int cc, int radius, char ch) {
    int x=0, y=radius, d=1-radius;
    while (x <= y) {
        plot(cr+y,cc+x,ch); plot(cr+y,cc-x,ch);
        plot(cr-y,cc+x,ch); plot(cr-y,cc-x,ch);
        plot(cr+x,cc+y,ch); plot(cr+x,cc-y,ch);
        plot(cr-x,cc+y,ch); plot(cr-x,cc-y,ch);
        if (d < 0) d += 2*x+3;
        else { d += 2*(x-y)+5; y--; }
        x++;
    }
}

void draw_triangle(int r0,int c0, int r1,int c1, int r2,int c2, char ch) {
    draw_line(r0,c0, r1,c1, ch);
    draw_line(r1,c1, r2,c2, ch);
    draw_line(r2,c2, r0,c0, ch);
}

/* ── Object store ── */
typedef enum { LINE, RECT, CIRCLE, TRIANGLE } ShapeType;

typedef struct {
    int id, active;
    ShapeType type;
    int x1,y1,x2,y2,x3,y3,r;
    char ch;
} Object;

Object objs[MAX_OBJ];
int obj_count = 0, next_id = 1;

void render_all() {
    canvas_clear();
    for (int i = 0; i < obj_count; i++) {
        Object *o = &objs[i];
        if (!o->active) continue;
        switch (o->type) {
            case LINE:     draw_line(o->y1,o->x1,o->y2,o->x2,o->ch); break;
            case RECT:     draw_rectangle(o->y1,o->x1,o->y2,o->x2,o->ch); break;
            case CIRCLE:   draw_circle(o->y1,o->x1,o->r,o->ch); break;
            case TRIANGLE: draw_triangle(o->y1,o->x1,o->y2,o->x2,o->y3,o->x3,o->ch); break;
        }
    }
}

int add_object(Object o) {
    if (obj_count >= MAX_OBJ) { printf("Max objects reached!\n"); return -1; }
    o.id = next_id++; o.active = 1;
    objs[obj_count++] = o;
    printf("Added object with ID %d\n", o.id);
    return o.id;
}

void delete_object(int id) {
    for (int i = 0; i < obj_count; i++)
        if (objs[i].id == id) { objs[i].active = 0; printf("Deleted ID %d\n", id); return; }
    printf("ID %d not found\n", id);
}

void modify_object(int id) {
    for (int i = 0; i < obj_count; i++) {
        if (objs[i].id != id || !objs[i].active) continue;
        Object *o = &objs[i];
        printf("Modifying %s (ID %d)\n",
            o->type==LINE?"Line":o->type==RECT?"Rectangle":o->type==CIRCLE?"Circle":"Triangle", id);
        switch (o->type) {
            case LINE:
                printf("New start row col: "); scanf("%d %d",&o->y1,&o->x1);
                printf("New end   row col: "); scanf("%d %d",&o->y2,&o->x2);
                break;
            case RECT:
                printf("New top-left  row col: "); scanf("%d %d",&o->y1,&o->x1);
                printf("New bot-right row col: "); scanf("%d %d",&o->y2,&o->x2);
                break;
            case CIRCLE:
                printf("New center row col: "); scanf("%d %d",&o->y1,&o->x1);
                printf("New radius: ");         scanf("%d",&o->r);
                break;
            case TRIANGLE:
                printf("New V1 row col: "); scanf("%d %d",&o->y1,&o->x1);
                printf("New V2 row col: "); scanf("%d %d",&o->y2,&o->x2);
                printf("New V3 row col: "); scanf("%d %d",&o->y3,&o->x3);
                break;
        }
        printf("Draw char (* or _): "); scanf(" %c",&o->ch);
        printf("Object %d modified!\n", id);
        return;
    }
    printf("ID %d not found\n", id);
}

void list_objects() {
    int any = 0;
    for (int i = 0; i < obj_count; i++) {
        if (!objs[i].active) continue;
        Object *o = &objs[i];
        printf("  ID %d | %s | char='%c'\n", o->id,
            o->type==LINE?"Line    ":o->type==RECT?"Rectangle":o->type==CIRCLE?"Circle   ":"Triangle ", o->ch);
        any = 1;
    }
    if (!any) printf("  (no objects)\n");
}

/* ── Menu ── */
void add_menu() {
    Object o = {0};
    char ch;
    printf("\nAdd Shape:\n  1. Line\n  2. Rectangle\n  3. Circle\n  4. Triangle\nChoice: ");
    int c; scanf("%d",&c);
    printf("Draw char (* or _): "); scanf(" %c",&ch); o.ch = ch;
    switch(c) {
        case 1:
            o.type = LINE;
            printf("Start row col: "); scanf("%d %d",&o.y1,&o.x1);
            printf("End   row col: "); scanf("%d %d",&o.y2,&o.x2);
            break;
        case 2:
            o.type = RECT;
            printf("Top-left  row col: "); scanf("%d %d",&o.y1,&o.x1);
            printf("Bot-right row col: "); scanf("%d %d",&o.y2,&o.x2);
            break;
        case 3:
            o.type = CIRCLE;
            printf("Center row col: "); scanf("%d %d",&o.y1,&o.x1);
            printf("Radius: ");         scanf("%d",&o.r);
            break;
        case 4:
            o.type = TRIANGLE;
            printf("V1 row col: "); scanf("%d %d",&o.y1,&o.x1);
            printf("V2 row col: "); scanf("%d %d",&o.y2,&o.x2);
            printf("V3 row col: "); scanf("%d %d",&o.y3,&o.x3);
            break;
        default: printf("Invalid\n"); return;
    }
    add_object(o);
}

int main() {
    canvas_clear();
    int choice, id;
    printf("=== 2D ASCII Graphics Editor ===\n");
    printf("Canvas: %d rows x %d cols | chars: * and _\n", ROWS, COLS);

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add shape\n");
        printf("2. Delete shape\n");
        printf("3. Modify shape\n");
        printf("4. List shapes\n");
        printf("5. Display canvas\n");
        printf("6. Clear canvas\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_menu(); render_all(); display(); break;
            case 2:
                printf("Enter ID to delete: "); scanf("%d",&id);
                delete_object(id); render_all(); display(); break;
            case 3:
                printf("Enter ID to modify: "); scanf("%d",&id);
                modify_object(id); render_all(); display(); break;
            case 4: list_objects(); break;
            case 5: render_all(); display(); break;
            case 6: canvas_clear(); obj_count=0; next_id=1; printf("Canvas cleared.\n"); display(); break;
            case 0: printf("Bye!\n"); return 0;
            default: printf("Invalid choice\n");
        }
    }
}
