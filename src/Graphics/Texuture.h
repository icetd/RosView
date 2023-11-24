#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

    void bind(int width, int height, uint8_t *rgbData);
    void bind(const std::string &path);
    void unbind() const;
    inline int getId() const { return m_ID; }
    inline int getWidth() const { return m_Width; }
    inline int getHeight() const { return m_Height; }

private:
	unsigned int m_ID;
    int m_Width, m_Height, m_BPP;
    unsigned char* m_LocalBuffer;
};

#endif