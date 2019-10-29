#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



// put two images to pgm format

typedef struct Image
{
    int width;
    int height;
    int **values;
} Image;
Image NewImage(int width, int height)
{
    int* values = (int*) calloc(width * height, sizeof(int));
    int ** rows = (int**) malloc(height*sizeof(int*));
    for (int i = 0; i < height; ++i)
    {
        rows[i] = values + i * width;
    }
    Image image = {.width = width, .height = height, .values = rows};
    return image;
}
Image getImage(char fileName[])
{
    FILE *file = fopen(fileName, "r");
    char *ligne = (char *)malloc(100 * sizeof(char));
    int width = 0, height = 0, state = 0, index = 0, pixel = 0;
    if (file != NULL)
    {
        while (fgets(ligne, 100, file) != NULL && state != 2)
        {
            state = sscanf(ligne, "%d %d", &width, &height);
        }
        int i, j;
        int buff=0;
        Image image = NewImage(width,height);
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                   fscanf(file, "%d", &buff);
                image.values[i][j] = buff;

            }
        }
        return image;
    }
}
void save(Image image, char *name)
{
    FILE *file = fopen(name, "w+");
    int i, j;
    if (file != NULL)
    {
        fprintf(file, "P2\n");
        fprintf(file, "%d %d\n", image.width, image.height);
        fprintf(file, "255\n");
        for (i = 0; i < image.height; i++)
        {
            for (j = 0; j < image.width; j++)
            {
                fprintf(file, (j == 0) ? "%d" : " %d", image.values[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}

int maximum(Image img)
{
    int max = 0;
    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            if (img.values[i][j] > max)
            {
                max = img.values[i][j];
            }
        }
    }
    return max;
}

int minimum(Image img)
{
    int min = 255;
    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            if (img.values[i][j] < min)
            {
                min = img.values[i][j];
            }
        }
    }
    return min;
}

int Luminance(Image image)
{
    int i, j, m = 0;
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            m += image.values[i][j];
        }
    }

    return m / (image.width * image.height);
}
float contraste1(Image image)
{
    int i, j, b = Luminance(image);
    float c = 0.0;
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            c += pow(image.values[i][j] - b, 2);
        }
    }
    c /= image.width * image.height;
    return sqrt(c);
}

float contrast2(Image image)
{
    int max = maximum(image), min = minimum(image);
    return 1.0 * (max - min)/(max + min);
}
Image inverse(Image image)
{
    int i, j;
    Image result = NewImage(image.width, image.height);
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            result.values[i][j] = 255 - image.values[i][j];
        }
    }
    save(result, "inverse");
    return result;
}
Image addition(Image image1, Image image2)
{
    int i, j, k;
    Image result = NewImage(image1.width, image1.height);
    for (i = 0; i < image1.height; i++)
    {
        for (j = 0; j < image1.width; j++)
        {
            k = image1.values[i][j] + image2.values[i][j];
            result.values[i][j] = k > 255 ? 255 : k;
        }
    }
    return result;
}
Image et(Image image1, Image image2)
{
    int i, j, k;
    Image result = NewImage(image1.width, image1.height);
    for (i = 0; i < image1.height; i++)
    {
        for (j = 0; j < image1.width; j++)
        {
            if (image1.values[i][j] < image2.values[i][j])
            {
                result.values[i][j] = image1.values[i][j];
            }
            else
            {
                result.values[i][j] = image2.values[i][j];
            }
        }
    }
    return result;
}
Image ou(Image image1, Image image2)
{
    int i, j, k;
    Image result = NewImage(image1.width, image1.height);
    for (i = 0; i < image1.height; i++)
    {
        for (j = 0; j < image1.width; j++)
        {
            if (image1.values[i][j] < image2.values[i][j])
            {
                result.values[i][j] = image2.values[i][j];
            }
            else
            {
                result.values[i][j] = image1.values[i][j];
            }
        }
    }
    return result;
}
Image substraction(Image image1, Image image2)
{
    int i, j, k;
    Image result = NewImage(image1.width, image1.height);
    for (i = 0; i < image1.height; i++)
    {
        for (j = 0; j < image2.width; j++)
        {
            k = image1.values[i][j] - image2.values[i][j];
            result.values[i][j] = k < 0 ? 0 : k;
        }
    }
    return result;
}

Image multiplication(Image image, float n)
{
    int i, j, k;
    Image result = NewImage(image.width, image.height);
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            k = (int)n * image.values[i][j];
            result.values[i][j] = k > 255 ? 255 : k;
        }
    }
    return result;
}

Image EgalisationHistogram(Image image)
{
    int i, j;
    Image result = NewImage(image.width, image.height);

    // calcul de l'histograme
    float *histogram = (float *)calloc(256, sizeof(float));
    float *density = (float *)calloc(256, sizeof(float));
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            histogram[image.values[i][j]]++;
        }
    }

    // normalisation de l'histograme
    for (i = 0; i < 256; i++)
    {
        histogram[i] /= (image.width * image.height);
    }

    // Densité de probabilité normalisé
    for (i = 0; i < 256; i++)
    {
        for (j = i; j < 256; j++)
        {
            density[j] += histogram[i];
        }
    }
    //Transformation des niveaux de gris en image
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            result.values[i][j] = (int)(density[image.values[i][j]] * 255);
        }
    }
    return result;
}
Image TransformationLineaire(Image image)
{
    int i, j, min = minimum(image), max = maximum(image);
    Image result = NewImage(image.width, image.height);
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            if (min == max)
            {
                result.values[i][j] = image.values[i][j];
            }
            else
            {
                result.values[i][j] = (int)(255.0 * (image.values[i][j] - min) / (max - min));
            }
        }
    }
    return result;
}

Image SaturationLineaire(Image image, int Smax, int Smin)
{
    int i, j, min = minimum(image), max = maximum(image);
    Image result = NewImage(image.width, image.height);
    if (min <= Smin && Smin <= Smax && Smax <= max)
    {
        for (i = 0; i < image.height; i++)
        {
            for (j = 0; j < image.width; j++)
            {
                if (Smin == Smax)
                {
                    result.values[i][j] = image.values[i][j];
                }
                else
                {
                    result.values[i][j] = (int)(255.0 * (image.values[i][j] - Smin) / (Smax - Smin));
                    if (result.values[i][j] <= 0)
                    {
                        result.values[i][j] = 0;
                    }
                    if (result.values[i][j] >= 255)
                    {
                        result.values[i][j] = 255;
                    }
                }
            }
        }
        return result;
    }
    return image;
}

int main(int argc, char **argv)
{
    int a;
    Image image1 = getImage("image1.pgm");
    Image image2=getImage("image2.pgm");
    printf(" image1  est de luminance  :%d\nLes contrastes sont : %d, %d", Luminance(image1), contraste1(image1), contrast2(image2));
    printf("\nVous pouvez parcourir la racine du projet pour voir les images generes \n");
    save(EgalisationHistogram(image1), "Egalisation_histogramme.pgm");
    save(TransformationLineaire(image1), "Transformation_lineaire.pgm");
    save(SaturationLineaire(image1, 234, 55), "Saturation_lineaire.pgm");
    save(inverse(image1), "inverse_image1.pgm");
    save(addition(image1, image2), "Addition.pgm");
    save(substraction(image1, image2), "soustraction.pgm");
    save(multiplication(image1, 2), "multiplication_image1.pgm");
    save(et(image1, image2), "interception.pgm");
    save(ou(image1, image2), "union.pgm");

    return 0;
}
