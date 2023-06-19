#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class Canvas {
public:
    Canvas(int width, int height, char emptySymbol) : width(width), height(height) {
        canvas.resize(height, std::vector<char>(width, emptySymbol));
    }

    void drawPixel(int x, int y, char symbol) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            canvas[y][x] = symbol;
        }
        else{
            std::cout<<"Bad coordinates of a figure!"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << toString();
            file.close();
        }
        else {
            std::cout << "Unable to open file: " << filename << std::endl;
        }
    }

    std::string toString() const {
        std::string result;
        for (const auto& row : canvas) {
            for (char pixel : row) {
                result += pixel;
            }
            result += '\n';
        }
        return result;
    }

    int getWidth() {
        return width;
    }
    int getHeight() {
        return height;
    }

private:
    int width;
    int height;
    std::vector<std::vector<char>> canvas;
};

class Figure {
public:
    virtual void draw(Canvas& canvas) = 0;
};

class Point : public Figure {
public:
    Point(int x, int y, char symbol) : x(x), y(y), symbol(symbol)  {}

    void draw(Canvas& canvas) override {
        canvas.drawPixel(x, y, symbol);
    }
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }

private:
    int x;
    int y;
    char symbol;
};

class Line : public Figure {
public:
    Line(const Point& startPoint, const Point& endPoint, char symbol)
        : startPoint(startPoint), endPoint(endPoint), symbol(symbol) {}

    void draw(Canvas& canvas) override {
        int x1 = startPoint.getX();
        int y1 = startPoint.getY();
        int x2 = endPoint.getX();
        int y2 = endPoint.getY();

        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            canvas.drawPixel(x1, y1, symbol);

            if (x1 == x2 && y1 == y2) {
                break;
            }

            int err2 = 2 * err;
            if (err2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (err2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

private:
    Point startPoint;
    Point endPoint;
    char symbol;
};

class Rectangle : public Figure {
public:
    Rectangle(int x, int y, int width, int height, char symbol)
        : x(x), y(y), width(width), height(height), symbol(symbol) {}

    void draw(Canvas& canvas) override {
        for (int i = x; i < x + width; ++i) {
            for (int j = y; j < y + height; ++j) {
                canvas.drawPixel(i, j, symbol);
            }
        }
    }

private:
    int x;
    int y;
    int width;
    int height;
    char symbol;
};

class Circle : public Figure {
public:
    Circle(int centerX, int centerY, int radius, char symbol)
        : centerX(centerX), centerY(centerY), radius(radius), symbol(symbol) {}

    void draw(Canvas& canvas) override {
        for (int i = 0; i < canvas.getWidth(); ++i) {
            for (int j = 0; j < canvas.getHeight(); ++j) {
                if (isInsideCircle(i, j)) {
                    canvas.drawPixel(i, j, symbol);
                }
            }
        }
    }

private:
    int centerX;
    int centerY;
    int radius;
    char symbol;

    bool isInsideCircle(int x, int y) const {
        int dx = x - centerX;
        int dy = y - centerY;
        return dx * dx + dy * dy <= radius * radius;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cout << "Unable to open file: " << configFile << std::endl;
        return 1;
    }

    int canvasWidth, canvasHeight;
    std::string outputFilename;
    char emptySymbol;
    std::vector<Figure*> figures;

    file >> canvasWidth >> canvasHeight >> outputFilename >> emptySymbol;

    std::string figureName;
    while (file >> figureName) {
        if (figureName == "Rectangle") {
            int x, y, width, height;
            char symbol;
            file >> x >> y >> width >> height >> symbol;
            figures.push_back(new Rectangle(x, y, width, height, symbol));
        }
        else if (figureName == "Circle") {
            int centerX, centerY, radius;
            char symbol;
            file >> centerX >> centerY >> radius >> symbol;
            figures.push_back(new Circle(centerX, centerY, radius, symbol));
        }
        else if(figureName == "Line") {
            int x1, x2, y1, y2;
            char symbol;
            file >> x1 >> y1 >> x2 >> y2 >> symbol;
            Point start(x1,y1,symbol);
            Point end(x2,y2,symbol);
            figures.push_back(new Line(start, end, symbol));
        }
        else if(figureName == "Point") {
            int x, y;
            char symbol;
            file >> x >> y >> symbol;
            figures.push_back(new Point(x, y, symbol));
        }
    }

    file.close();

    Canvas canvas(canvasWidth, canvasHeight, emptySymbol);

    for (Figure* figure : figures) {
        figure->draw(canvas);
    }

    canvas.saveToFile(outputFilename);

    for (Figure* figure : figures) {
        delete figure;
    }
}