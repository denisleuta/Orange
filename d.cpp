#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Структура, представляющая вершину графа
struct Vertex {
    double x;
    double y;
    vector<int> edges;
};

// Функция для рисования линии между двумя точками на изображении
void drawLine(int x0, int y0, int x1, int y1, vector<vector<bool>>& image) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true) {
        image[y0][x0] = true;
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

// Функция для рисования графа
void drawGraph(const vector<Vertex>& vertices, const string& filename) {
    // Находим максимальные и минимальные значения координат
    double minX = numeric_limits<double>::max();
    double minY = numeric_limits<double>::max();
    double maxX = numeric_limits<double>::min();
    double maxY = numeric_limits<double>::min();

    for (const auto& vertex : vertices) {
        minX = min(minX, vertex.x);
        minY = min(minY, vertex.y);
        maxX = max(maxX, vertex.x);
        maxY = max(maxY, vertex.y);
    }

    // Определяем размер изображения
    int width = ceil(maxX - minX) + 100;
    int height = ceil(maxY - minY) + 100;

    // Создаём изображение
    vector<vector<bool>> image(height, vector<bool>(width, false));

    // Рисуем рёбра
    for (const auto& vertex : vertices) {
        for (const auto& edge : vertex.edges) {
            drawLine(vertex.x - minX + 50, vertex.y - minY + 50, vertices[edge].x - minX + 50, vertices[edge].y - minY + 50, image);
        }
    }

    // Рисуем вершины
    const int vertexRadius = 5; // Радиус вершины
    for (const auto& vertex : vertices) {
        for (int i = -vertexRadius; i <= vertexRadius; ++i) {
            for (int j = -vertexRadius; j <= vertexRadius; ++j) {
                int x = vertex.x - minX + 50 + i;
                int y = vertex.y - minY + 50 + j;
                if (x >= 0 && x < width && y >= 0 && y < height)
                    image[y][x] = true;
            }
        }
    }

    // Сохраняем изображение в файл BMP
    ofstream outFile(filename, ios::out | ios::binary);
    if (!outFile) {
        cerr << "Could not open file for writing!";
        return;
    }

    // BMP header
    int widthInBytes = ((width + 31) / 32) * 4; // BMP image width is padded with 0s to be a multiple of 4 bytes
    int dataSize = widthInBytes * height;
    int fileSize = 54 + dataSize;
    char header[54] = {
        'B', 'M',                          // FileType
        (char)(fileSize), (char)(fileSize >> 8), (char)(fileSize >> 16), (char)(fileSize >> 24), // FileSize
        0, 0, 0, 0,                         // Reserved1
        54, 0, 0, 0,                        // DataOffset
        40, 0, 0, 0,                        // Size
        (char)(width), (char)(width >> 8), (char)(width >> 16), (char)(width >> 24), // Width
        (char)(height), (char)(height >> 8), (char)(height >> 16), (char)(height >> 24), // Height
        1, 0,                               // Planes
        24, 0,                              // Bits per pixel
        0, 0, 0, 0,                         // Compression
        (char)(dataSize), (char)(dataSize >> 8), (char)(dataSize >> 16), (char)(dataSize >> 24), // ImageSize
        19, 11, 0, 0,                       // XpixelsPerM
        19, 11, 0, 0,                       // YpixelsPerM
        0, 0, 0, 0,                         // ColorsUsed
        0, 0, 0, 0                          // ColorsImportant
    };

    outFile.write(header, 54);

    // Write image data
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            char pixelData[3] = { 0, 0, 0 };
            if (image[y][x]) {
                pixelData[0] = 255; // Blue
                pixelData[1] = 255; // Green
                pixelData[2] = 255; // Red
            }
            outFile.write(pixelData, 3);
        }
        // Write padding if needed
        for (int p = 0; p < (4 - (width % 4)) % 4; ++p)
            outFile.put(0);
    }

    outFile.close();
}

int main() {
    // Создаём граф с 500 вершинами
    vector<Vertex> graph;
    const int numVertices = 500;
    const double radius = 600;
    const double centerX = 1280; // 2560 / 2
    const double centerY = 960;  // 1920 / 2

    for (int i = 0; i < numVertices; ++i) {
        double angle = 2 * M_PI * i / numVertices;
        double x = centerX + radius * cos(angle);
        double y = centerY + radius * sin(angle);
        graph.push_back({ x, y });
    }

    // Соединяем каждую вершину с двумя соседними
    for (int i = 0; i < numVertices; ++i) {
        graph[i].edges.push_back((i + 1) % numVertices);
        graph[i].edges.push_back((i - 1 + numVertices) % numVertices);
    }

    // Создаём изображение графа и сохраняем его
    drawGraph(graph, "planar_graph.bmp");

    return 0;
}
