#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* ─── Canvas Dimensions ─────────────────────────────────────── */
#define ROWS        25
#define COLS        80
#define MAX_OBJECTS 100
/* ─── Shape Type Identifiers ────────────────────────────────── */
typedef enum {
    SHAPE_CIRCLE    = 1,
    SHAPE_RECTANGLE = 2,
    SHAPE_LINE      = 3,
    SHAPE_TRIANGLE  = 4
} ShapeType;
/* ─── Shape Parameter Structs ───────────────────────────────── */
typedef struct { int cx, cy, radius;              } CircleData;
typedef struct { int x,  y,  width,  height;      } RectData;
typedef struct { int x1, y1, x2, y2;              } LineData;
typedef struct { int x1, y1, x2, y2, x3, y3;     } TriData;
/* ─── Generic Object ────────────────────────────────────────── */
typedef struct {
    int       id;
    ShapeType type;
    union {
        CircleData circle;
        RectData   rect;
        LineData   line;
        TriData    tri;
    } data;
} Object;
/* ─── Globals ───────────────────────────────────────────────── */
static char   canvas[ROWS][COLS];
static Object objects[MAX_OBJECTS];
static int    objCount = 0;
static int    nextId   = 1;
/* ════════════════════════════════════════════════════════════ */
/*                    CANVAS UTILITIES                          */
/* ════════════════════════════════════════════════════════════ */
/* Reset canvas to underscore fill */
void initCanvas(void) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = '_';
}
/* Safe pixel plot — ignores out-of-bounds coordinates */
void plotPixel(int row, int col) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        canvas[row][col] = '*';
}
/* Print the full canvas with row/column guides */
void displayCanvas(void) {
    /* Column ruler (tens digit) */
    printf("\n    ");
    for (int c = 0; c < COLS; c++)
        printf("%d", (c / 10) % 10);
    printf("\n");
    /* Column ruler (units digit) */
    printf("    ");
    for (int c = 0; c < COLS; c++)
        printf("%d", c % 10);
    printf("\n");
    /* Separator */
    printf("   +");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
    /* Canvas rows */
    for (int r = 0; r < ROWS; r++) {
        printf("%2d |", r);
        for (int c = 0; c < COLS; c++)
            printf("%c", canvas[r][c]);
        printf("|\n");
    }
    /* Separator */
    printf("   +");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
    printf("    Cols 0..%d   Rows 0..%d\n\n", COLS - 1, ROWS - 1);
}
/* ════════════════════════════════════════════════════════════ */
/*                    DRAWING ALGORITHMS                        */
/* ════════════════════════════════════════════════════════════ */
/*
 * Bresenham's Line Algorithm
 * Draws a line from (x1,y1) to (x2,y2).
 * x = column, y = row.
 */
void renderLine(int x1, int y1, int x2, int y2) {
    int dx =  abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx + dy;
    while (1) {
        plotPixel(y1, x1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}
/*
 * Midpoint Circle Algorithm
 * Draws a circle with center (cx,cy) and given radius.
 * cx = column, cy = row.
 */
void renderCircle(int cx, int cy, int radius) {
    if (radius <= 0) return;
    int x = 0, y = radius;
    int d = 1 - radius;
    while (x <= y) {
        /* 8-way symmetry */
        plotPixel(cy + y, cx + x);
        plotPixel(cy + y, cx - x);
        plotPixel(cy - y, cx + x);
        plotPixel(cy - y, cx - x);
        plotPixel(cy + x, cx + y);
        plotPixel(cy + x, cx - y);
        plotPixel(cy - x, cx + y);
        plotPixel(cy - x, cx - y);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}
/*
 * Rectangle: top-left corner (x,y), width, height.
 * x = column, y = row.
 */
void renderRectangle(int x, int y, int w, int h) {
    /* Top and bottom horizontal edges */
    for (int c = x; c <= x + w; c++) {
        plotPixel(y,     c);
        plotPixel(y + h, c);
    }
    /* Left and right vertical edges */
    for (int r = y; r <= y + h; r++) {
        plotPixel(r, x);
        plotPixel(r, x + w);
    }
}
/*
 * Triangle: three vertices using three Bresenham lines.
 */
void renderTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    renderLine(x1, y1, x2, y2);
    renderLine(x2, y2, x3, y3);
    renderLine(x3, y3, x1, y1);
}
/* ════════════════════════════════════════════════════════════ */
/*                    OBJECT MANAGEMENT                         */
/* ════════════════════════════════════════════════════════════ */
/* Redraw canvas from scratch using all stored objects */
void redrawAll(void) {
    initCanvas();
    for (int i = 0; i < objCount; i++) {
        Object *o = &objects[i];
        switch (o->type) {
            case SHAPE_CIRCLE:
                renderCircle(o->data.circle.cx,
                             o->data.circle.cy,
                             o->data.circle.radius);
                break;
            case SHAPE_RECTANGLE:
                renderRectangle(o->data.rect.x,
                                o->data.rect.y,
                                o->data.rect.width,
                                o->data.rect.height);
                break;
            case SHAPE_LINE:
                renderLine(o->data.line.x1, o->data.line.y1,
                           o->data.line.x2, o->data.line.y2);
                break;
            case SHAPE_TRIANGLE:
                renderTriangle(o->data.tri.x1, o->data.tri.y1,
                               o->data.tri.x2, o->data.tri.y2,
                               o->data.tri.x3, o->data.tri.y3);
                break;
        }
    }
}
/* Find the array index of an object by its ID, returns -1 if not found */
int findById(int id) {
    for (int i = 0; i < objCount; i++)
        if (objects[i].id == id) return i;
    return -1;
}
/* Print a single object's details */
void printObject(const Object *o) {
    printf("  [ID %d] ", o->id);
    switch (o->type) {
        case SHAPE_CIRCLE:
            printf("Circle    — center=(%d,%d)  radius=%d\n",
                   o->data.circle.cx, o->data.circle.cy,
                   o->data.circle.radius);
            break;
        case SHAPE_RECTANGLE:
            printf("Rectangle — top-left=(%d,%d)  width=%d  height=%d\n",
                   o->data.rect.x, o->data.rect.y,
                   o->data.rect.width, o->data.rect.height);
            break;
        case SHAPE_LINE:
            printf("Line      — (%d,%d) -> (%d,%d)\n",
                   o->data.line.x1, o->data.line.y1,
                   o->data.line.x2, o->data.line.y2);
            break;
        case SHAPE_TRIANGLE:
            printf("Triangle  — (%d,%d) , (%d,%d) , (%d,%d)\n",
                   o->data.tri.x1, o->data.tri.y1,
                   o->data.tri.x2, o->data.tri.y2,
                   o->data.tri.x3, o->data.tri.y3);
            break;
    }
}
/* List all objects currently on the canvas */
void listObjects(void) {
    if (objCount == 0) {
        printf("  (canvas is empty — no objects added yet)\n");
        return;
    }
    printf("\n  %-5s %-12s Details\n", "ID", "Shape");
    printf("  ------------------------------------------------\n");
    for (int i = 0; i < objCount; i++)
        printObject(&objects[i]);
    printf("  ------------------------------------------------\n");
    printf("  Total: %d object(s)\n", objCount);
}
/* ════════════════════════════════════════════════════════════ */
/*                    ADD OPERATIONS                            */
/* ════════════════════════════════════════════════════════════ */
void addCircle(void) {
    if (objCount >= MAX_OBJECTS) { printf("  Error: object limit reached.\n"); return; }
    int cx, cy, r;
    printf("\n  -- Add Circle --\n");
    printf("  Enter center column (0-%d): ", COLS - 1); scanf("%d", &cx);
    printf("  Enter center row    (0-%d): ", ROWS - 1); scanf("%d", &cy);
    printf("  Enter radius: ");                          scanf("%d", &r);
    if (r <= 0) { printf("  Error: radius must be > 0.\n"); return; }
    Object *o = &objects[objCount++];
    o->id              = nextId++;
    o->type            = SHAPE_CIRCLE;
    o->data.circle.cx  = cx;
    o->data.circle.cy  = cy;
    o->data.circle.radius = r;
    redrawAll();
    printf("  Circle added with ID %d.\n", o->id);
}
void addRectangle(void) {
    if (objCount >= MAX_OBJECTS) { printf("  Error: object limit reached.\n"); return; }
    int x, y, w, h;
    printf("\n  -- Add Rectangle --\n");
    printf("  Enter top-left column (0-%d): ", COLS - 1); scanf("%d", &x);
    printf("  Enter top-left row    (0-%d): ", ROWS - 1); scanf("%d", &y);
    printf("  Enter width  (>0): ");                       scanf("%d", &w);
    printf("  Enter height (>0): ");                       scanf("%d", &h);
    if (w <= 0 || h <= 0) { printf("  Error: width and height must be > 0.\n"); return; }
    Object *o = &objects[objCount++];
    o->id            = nextId++;
    o->type          = SHAPE_RECTANGLE;
    o->data.rect.x   = x;
    o->data.rect.y   = y;
    o->data.rect.width  = w;
    o->data.rect.height = h;
    redrawAll();
    printf("  Rectangle added with ID %d.\n", o->id);
}
void addLine(void) {
    if (objCount >= MAX_OBJECTS) { printf("  Error: object limit reached.\n"); return; }
    int x1, y1, x2, y2;
    printf("\n  -- Add Line --\n");
    printf("  Enter start column (0-%d): ", COLS - 1); scanf("%d", &x1);
    printf("  Enter start row    (0-%d): ", ROWS - 1); scanf("%d", &y1);
    printf("  Enter end   column (0-%d): ", COLS - 1); scanf("%d", &x2);
    printf("  Enter end   row    (0-%d): ", ROWS - 1); scanf("%d", &y2);
    Object *o = &objects[objCount++];
    o->id           = nextId++;
    o->type         = SHAPE_LINE;
    o->data.line.x1 = x1;  o->data.line.y1 = y1;
    o->data.line.x2 = x2;  o->data.line.y2 = y2;
    redrawAll();
    printf("  Line added with ID %d.\n", o->id);
}
void addTriangle(void) {
    if (objCount >= MAX_OBJECTS) { printf("  Error: object limit reached.\n"); return; }
    int x1,y1,x2,y2,x3,y3;
    printf("\n  -- Add Triangle --\n");
    printf("  Enter vertex 1 column (0-%d): ", COLS-1); scanf("%d",&x1);
    printf("  Enter vertex 1 row    (0-%d): ", ROWS-1); scanf("%d",&y1);
    printf("  Enter vertex 2 column (0-%d): ", COLS-1); scanf("%d",&x2);
    printf("  Enter vertex 2 row    (0-%d): ", ROWS-1); scanf("%d",&y2);
    printf("  Enter vertex 3 column (0-%d): ", COLS-1); scanf("%d",&x3);
    printf("  Enter vertex 3 row    (0-%d): ", ROWS-1); scanf("%d",&y3);
    Object *o = &objects[objCount++];
    o->id          = nextId++;
    o->type        = SHAPE_TRIANGLE;
    o->data.tri.x1 = x1; o->data.tri.y1 = y1;
    o->data.tri.x2 = x2; o->data.tri.y2 = y2;
    o->data.tri.x3 = x3; o->data.tri.y3 = y3;
    redrawAll();
    printf("  Triangle added with ID %d.\n", o->id);
}
/* ════════════════════════════════════════════════════════════ */
/*                    DELETE OPERATION                          */
/* ════════════════════════════════════════════════════════════ */
void deleteObject(void) {
    printf("\n  -- Delete Object --\n");
    listObjects();
    if (objCount == 0) return;
    int id;
    printf("\n  Enter ID to delete: "); scanf("%d", &id);
    int idx = findById(id);
    if (idx == -1) {
        printf("  Error: no object with ID %d found.\n", id);
        return;
    }
    /* Shift objects left to fill the gap */
    for (int i = idx; i < objCount - 1; i++)
        objects[i] = objects[i + 1];
    objCount--;
    redrawAll();
    printf("  Object ID %d deleted.\n", id);
}
/* ════════════════════════════════════════════════════════════ */
/*                    MODIFY OPERATION                          */
/* ════════════════════════════════════════════════════════════ */
void modifyObject(void) {
    printf("\n  -- Modify Object --\n");
    listObjects();
    if (objCount == 0) return;
    int id;
    printf("\n  Enter ID to modify: "); scanf("%d", &id);
    int idx = findById(id);
    if (idx == -1) {
        printf("  Error: no object with ID %d found.\n", id);
        return;
    }
    Object *o = &objects[idx];
    printf("\n  Current state: ");
    printObject(o);
    printf("\n  Enter new parameters:\n");
    switch (o->type) {
        case SHAPE_CIRCLE: {
            int cx, cy, r;
            printf("  New center column (0-%d): ", COLS-1); scanf("%d",&cx);
            printf("  New center row    (0-%d): ", ROWS-1); scanf("%d",&cy);
            printf("  New radius (>0)         : ");          scanf("%d",&r);
            if (r <= 0) { printf("  Error: radius must be > 0.\n"); return; }
            o->data.circle.cx     = cx;
            o->data.circle.cy     = cy;
            o->data.circle.radius = r;
            break;
        }
        case SHAPE_RECTANGLE: {
            int x, y, w, h;
            printf("  New top-left column (0-%d): ", COLS-1); scanf("%d",&x);
            printf("  New top-left row    (0-%d): ", ROWS-1); scanf("%d",&y);
            printf("  New width  (>0)           : ");          scanf("%d",&w);
            printf("  New height (>0)           : ");          scanf("%d",&h);
            if (w <= 0 || h <= 0) { printf("  Error: width/height must be > 0.\n"); return; }
            o->data.rect.x      = x;
            o->data.rect.y      = y;
            o->data.rect.width  = w;
            o->data.rect.height = h;
            break;
        }
        case SHAPE_LINE: {
            int x1,y1,x2,y2;
            printf("  New start column (0-%d): ", COLS-1); scanf("%d",&x1);
            printf("  New start row    (0-%d): ", ROWS-1); scanf("%d",&y1);
            printf("  New end   column (0-%d): ", COLS-1); scanf("%d",&x2);
            printf("  New end   row    (0-%d): ", ROWS-1); scanf("%d",&y2);
            o->data.line.x1 = x1; o->data.line.y1 = y1;
            o->data.line.x2 = x2; o->data.line.y2 = y2;
            break;
        }
        case SHAPE_TRIANGLE: {
            int x1,y1,x2,y2,x3,y3;
            printf("  New vertex 1 column (0-%d): ", COLS-1); scanf("%d",&x1);
            printf("  New vertex 1 row    (0-%d): ", ROWS-1); scanf("%d",&y1);
            printf("  New vertex 2 column (0-%d): ", COLS-1); scanf("%d",&x2);
            printf("  New vertex 2 row    (0-%d): ", ROWS-1); scanf("%d",&y2);
            printf("  New vertex 3 column (0-%d): ", COLS-1); scanf("%d",&x3);
            printf("  New vertex 3 row    (0-%d): ", ROWS-1); scanf("%d",&y3);
            o->data.tri.x1=x1; o->data.tri.y1=y1;
            o->data.tri.x2=x2; o->data.tri.y2=y2;
            o->data.tri.x3=x3; o->data.tri.y3=y3;
            break;
        }
    }
    redrawAll();
    printf("  Object ID %d modified.\n", id);
}
/* ════════════════════════════════════════════════════════════ */
/*                    MENU SYSTEM                               */
/* ════════════════════════════════════════════════════════════ */
void printBanner(void) {
    printf("\n");
    printf("  +==========================================+\n");
    printf("  |       2D CHARACTER GRAPHICS EDITOR       |\n");
    printf("  |   Canvas: %2d rows x %2d cols              |\n", ROWS, COLS);
    printf("  |   Fill: '_'   Draw: '*'                  |\n");
    printf("  +==========================================+\n");
}
void printMainMenu(void) {
    printf("\n");
    printf("  +-----------------------------+\n");
    printf("  |         MAIN MENU           |\n");
    printf("  |-----------------------------|");
    printf("\n  |  1. Display Canvas          |\n");
    printf("  |  2. Add Object              |\n");
    printf("  |  3. Delete Object           |\n");
    printf("  |  4. Modify Object           |\n");
    printf("  |  5. List All Objects        |\n");
    printf("  |  6. Clear Canvas            |\n");
    printf("  |  7. Exit                    |\n");
    printf("  +-----------------------------+\n");
    printf("  Choice: ");
}
void printAddMenu(void) {
    printf("\n");
    printf("  +-----------------------------+\n");
    printf("  |         ADD OBJECT          |\n");
    printf("  |-----------------------------|");
    printf("\n  |  1. Circle                  |\n");
    printf("  |  2. Rectangle               |\n");
    printf("  |  3. Line                    |\n");
    printf("  |  4. Triangle                |\n");
    printf("  |  0. Back                    |\n");
    printf("  +-----------------------------+\n");
    printf("  Choice: ");
}
void handleAddMenu(void) {
    int choice;
    printAddMenu();
    if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); return; }
    switch (choice) {
        case 1: addCircle();    break;
        case 2: addRectangle(); break;
        case 3: addLine();      break;
        case 4: addTriangle();  break;
        case 0: break;
        default: printf("  Invalid choice.\n");
    }
}
/* ════════════════════════════════════════════════════════════ */
/*                         MAIN                                 */
/* ════════════════════════════════════════════════════════════ */
int main(void) {
    initCanvas();
    printBanner();
    int choice;
    do {
        printMainMenu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');  /* flush bad input */
            choice = -1;
        }
        switch (choice) {
            case 1: displayCanvas();                        break;
            case 2: handleAddMenu();                        break;
            case 3: deleteObject();                         break;
            case 4: modifyObject();                         break;
            case 5:
                printf("\n  -- All Objects --\n");
                listObjects();
                break;
            case 6:
                objCount = 0;
                nextId   = 1;
                initCanvas();
                printf("  Canvas cleared.\n");
                break;
            case 7:
                printf("\n  Goodbye!\n\n");
                break;
            default:
                printf("  Invalid choice. Please enter 1-7.\n");
        }
    } while (choice != 7);
    return 0;
}