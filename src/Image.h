//
// Created by Andreas Royset on 7/6/25.
//

#ifndef IMAGE_H
#define IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "int2.h"
#include "float3.h"

inline float linearizeF(float x) {
    x/=255;
    x = float(fmin(fmax(x, 0), 1));
    x = float(pow(x, 1/2.2)); // gamma correction
    return x*255;
}
inline float acesF(const float& x) {
    const float a = x * (x + 0.0245786f) - 0.000090537f;
    const float b = x * 0.983729f + 0.4329510f;
    return a / b;
}

class Image {
    int2 size;
    std::vector<float>* data;

    public:
    Image() {
        size.x = size.y = 0;
        data = nullptr;
    }
    Image(const int width, const int height) {
        size.x = width;
        size.y = height;
        data = new std::vector<float>(size.x * size.y * 3);
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) = 0;
        }
    }
    Image(const int width, const int height, std::vector<float>* data) {
        size.x = width;
        size.y = height;
        this->data = data;
    }
    Image(const Image& image) {
        size.x = image.size.x;
        size.y = image.size.y;
        data = new std::vector<float>(size.x * size.y * 3);
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) = image.data->at(i);
        }
    }
    explicit Image(const std::string& filename) {
        int width, height, channels;
        const unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

        this->data = new std::vector<float>(width * height * 3);
        this->size.x = width;
        this->size.y = height;
        for (int i = 0; i < width * height; i++) {
            this->data->at(i*3) = data[i*channels];
            this->data->at(i*3+1) = data[i*channels+1];
            this->data->at(i*3+2) = data[i*channels+2];
        }
    }
    ~Image(){
        delete data;
    }

    void operator += (const Image& other) const {
        if (other.size.x < size.x or other.size.y < size.y) {
            std::cout << this->size.x << " " << this->size.y << std::endl;
            std::cout << other.size.x << " " << other.size.y << std::endl;
            std::cerr << "Image too small" << std::endl;
        }
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) += other.data->at(i);
        }
    }
    void operator += (const float3 offset) const {
        for (int i = 0; i < size.x * size.y; i++) {
            if (data->at(i*3)<-offset.x) data->at(i*3) = 0;
            else data->at(i*3) += offset.x;

            if (data->at(i*3+1)<-offset.y) data->at(i*3+1) = 0;
            else data->at(i*3+1) += offset.y;

            if (data->at(i*3+2)<-offset.z) data->at(i*3+2) = 0;
            else data->at(i*3+2) += offset.z;
        }
    }
    void operator *= (const float3 offset) const {
        for (int i = 0; i < size.x * size.y; i++) {
            data->at(i*3) = data->at(i*3)*offset.x;
            data->at(i*3 + 1) = data->at(i*3+1)*offset.y;
            data->at(i*3 + 2) = data->at(i*3+2)*offset.z;
        }
    }

    void clear() const {
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) = 0;
        }
    }
    void clear(const int2 size) {
        this->size = size;
        this->data = new std::vector<float>(size.x * size.y * 3);
    }
    void resize(const int2 size) {
        if (size == this->size) {return;}
        const Image image = *this;
        this->size.x = size.x;
        this->size.y = size.y;
        this->data = new std::vector<float>(size.x * size.y * 3);
        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                write(x,y,image.read(x,y));
            }
        }
    }
    void makePng(const std::string& filename) const {
        auto* newData = new unsigned char[size.x * size.y * 3];
        for (int i = 0; i < size.x*size.y*3; i++) {
            float color = data->at(i);
            if (color > 255) color = 255;
            newData[i] = int(color);
        }
        stbi_write_png(filename.c_str(), size.x, size.y, 3, newData, size.x * 3);
        delete[] newData;
    }
    void blur(const int r) const {
        const Image image = *this;
        const int r2 = r*r;
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                float3 color;
                float samples = 0;

                for (int oy = -r; oy <= r; oy++) {
                    int dis2 = r2+oy*oy;
                    if (oy+y < 0 or oy+y >= size.y) continue;
                    for (int ox = -r; ox <= r; ox++) {
                        if (ox+x < 0 or ox+x >= size.x) {dis2 += 2*ox+1; continue;}
                        if (dis2 > r2) {dis2 += 2*ox+1; continue;}

                        color += image.read(x+ox, y+oy) * float(r2-dis2);
                        samples += float(r2-dis2);

                        dis2 += 2*ox+1;
                    }
                }

                color /= samples;
                color.clamp(0, 255);
                write(x, y, color);
            }
        }
    }
    void blur(const std::vector<float>& kernel) const {
        Hblur(kernel);
        Vblur(kernel);
    }
    void fastBoxBlur(const int r) const {
        Hblur(r);
        Vblur(r);
    }
    void Hblur(const int r) const {
        Image image = *this;
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                float3 color;
                float samples = 0;
                for (int ox = -r; ox <= r; ox++) {
                    if (ox + x < 0 or ox + x >= size.x) continue;
                    color += image.read(x+ox, y);
                    samples++;
                }
                color /= samples;
                color.clamp(0, 255);
                write(x, y, color);
            }
        }
    }
    void Vblur(const int r) const {
        Image image = *this;
        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                float3 color;
                float samples = 0;
                for (int oy = -r; oy <= r; oy++) {
                    if (oy + y < 0 or oy + y >= size.x) continue;
                    color += image.read(x, y+oy);
                    samples++;
                }
                color /= samples;
                color.clamp(0, 255);
                write(x, y, color);
            }
        }
    }
    void Hblur(const std::vector<float>& kernel) const {
        Image image = *this;
        int r = int(kernel.size())/2;
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                float3 color;
                float samples = 0;
                for (int ox = -r; ox <= r; ox++) {
                    if (ox + x < 0 or ox + x >= size.x) continue;
                    const float weight = kernel.at(r+ox);
                    color += image.read(x+ox, y) * weight;
                    samples += weight;
                }
                color /= samples;
                color.clamp(0, 255);
                write(x, y, color);
            }
        }
    }
    void Vblur(const std::vector<float>& kernel) const {
        Image image = *this;
        int r = int(kernel.size())/2;
        for (int x = 0; x < size.x; x++) {
            for (int y = 0; y < size.y; y++) {
                float3 color;
                float samples = 0;
                for (int oy = -r; oy <= r; oy++) {
                    if (oy + y < 0 or oy + y >= size.x) continue;
                    const float weight = kernel.at(r+oy);
                    color += image.read(x, y+oy) * weight;
                    samples += weight;
                }
                color /= samples;
                color.clamp(0, 255);
                write(x, y, color);
            }
        }
    }
    void downsample() {
        const Image image = *this;
        this->clear({int(ceil(float(image.size.x)/2)), int(ceil(float(image.size.y)/2))});
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                float3 color;
                for (int oy = 0; oy <= 1; oy++) {
                    for (int ox = 0; ox <= 1; ox++) {
                        color += image.read(2*x+ox, 2*y+oy);
                    }
                }
                write(x, y, color * 0.25f);
            }
        }
    }
    void upsample() {
        const Image image = *this;
        this->clear({image.size.x*2, image.size.y*2});
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                const float3 tl = image.read(x/2, y/2);
                const float3 tr = image.read(x/2+1, y/2);
                const float3 bl = image.read(x/2, y/2+1);
                const float3 br = image.read(x/2+1, y/2+1);

                float intX;
                const float fracX = std::modf(float(x)/2, &intX);
                const float3 t = tl.lerp(tr, fracX);
                const float3 b = bl.lerp(br, fracX);

                float intY;
                const float fracY = std::modf(float(y)/2, &intY);
                const float3 color = t.lerp(b, fracY);

                write(x,y, color);
            }
        }
    }
    void apply(const std::function<float3(float3)>& func) const {
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                write(x,y, func(read(x,y)));
            }
        }
    }
    void apply(const std::function<float(float)>& func) const {
        for (int i = 0; i < size.x*size.y*3; i++) {
            data->at(i) = func(data->at(i));
        }
    }
    void clamp(const float min, const float max) const {
        for (int i = 0; i < size.x*size.y*3; i++) {
            if (data->at(i) < min) data->at(i) = min;
            if (data->at(i) > max) data->at(i) = max;
        }
    }

    void divide(const std::vector<int> &samples) const {
        for (int i = 0; i < size.x * size.y; i++) {
            if (samples.at(i) == 0) {
                data->at(i*3) = 0;
                data->at(i*3 + 1) = 0;
                data->at(i*3 + 2) = 0;
                continue;
            }
            const float inv = 1.0f/float(samples.at(i));
            data->at(i*3) *= inv;
            data->at(i*3+1) *= inv;
            data->at(i*3+2) *= inv;
        }
    }
    void linearize() const {
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) = linearizeF(data->at(i));
        }
    }
    void aces() const {
        for (int i = 0; i < size.x * size.y * 3; i++) {
            data->at(i) = acesF(data->at(i));
        }
    }

    void write(const int x, const int y, const float3 color) const {
        const int index = y * size.x + x;
        data->at(index*3) = color.x;
        data->at(index*3+1) = color.y;
        data->at(index*3+2) = color.z;
    }
    void add(const int x, const int y, const float3 color) const {
        const int index = y * size.x + x;
        data->at(index*3) += color.x;
        data->at(index*3+1) += color.y;
        data->at(index*3+2) += color.z;
    }
    [[nodiscard]] float3 read(int x, int y) const {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x >= size.x) x = size.x - 1;
        if (y >= size.y) y = size.y - 1;
        float3 color;
        const int index = y * size.x + x;
        color.x = data->at(index*3);
        color.y = data->at(index*3+1);
        color.z = data->at(index*3+2);
        return color;
    }
    [[nodiscard]] int2 getSize() const {
        return size;
    }

    [[nodiscard]] std::vector<float>* getData() const {
        return data;
    }
};

#endif //IMAGE_H
