# 2D Character Graphics Editor

A lightweight, interactive command-line 2D graphics editor written in C. This program allows users to draw, modify, delete, and list shapes on an 80x25 character-based grid canvas using Bresenham's Line Algorithm and the Midpoint Circle Algorithm.

## Features

- **Interactive Console Menu**: Easy-to-use menu system for drawing and editing shapes.
- **Canvas Rendering**: 80 columns by 25 rows grid display with numeric axis rulers.
- **Vector Shapes**:
  - **Circle**: Drawn with the Midpoint Circle Algorithm.
  - **Rectangle**: Outlined vector borders.
  - **Line**: Drawn using Bresenham's Line Algorithm.
  - **Triangle**: Drawn by connecting three vertices with lines.
- **Object Management**: 
  - Dynamic shape creation with unique IDs.
  - List details of all objects on the canvas.
  - Modify shape parameters in-place.
  - Delete shapes by ID.
  - Canvas redraw from scratch.

## Project Structure

- [main.c](file:///c:/Users/supra/OneDrive/Desktop/Mini%20C/main.c): Contains the source code of the graphics editor, including shape drawing math and CLI menu logic.
- [C-program mini project.txt](file:///c:/Users/supra/OneDrive/Desktop/Mini%20C/C-program%20mini%20project.txt): Original project description and code source.

## Getting Started

### Prerequisites

You need a C compiler (such as `gcc`, `clang`, or MSVC) installed on your system.

### Compilation

Compile the project using a standard terminal command:

```bash
gcc main.c -o graphics_editor
```

### Running the Editor

Execute the compiled binary:

```bash
./graphics_editor
```

## How to Use

1. **Display Canvas**: Prints the current state of the 80x25 character grid.
2. **Add Object**: Choose a shape (Circle, Rectangle, Line, Triangle) and input its dimensions/coordinates.
3. **Delete Object**: Input the unique ID of the object you wish to remove from the canvas.
4. **Modify Object**: Select an object by ID and update its parameters (coordinates, size).
5. **List All Objects**: Shows a table detailing all shapes currently placed on the canvas.
6. **Clear Canvas**: Removes all objects and resets the editor.
7. **Exit**: Closes the application.
