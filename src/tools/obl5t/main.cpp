#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>
#include "../../shared/stdafx.h"
#include "../../shared/FileWrap.h"
#include "../../shared/FrameSet.h"
#include "../../shared/Frame.h"
#include "../../shared/ss_version.h"

#define qDebug printf

/* These describe the color_type field in png_info. */
/* color type masks */
#define PNG_COLOR_MASK_PALETTE 1
#define PNG_COLOR_MASK_COLOR 2
#define PNG_COLOR_MASK_ALPHA 4

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
/* aliases */
#define PNG_COLOR_TYPE_RGBA PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA PNG_COLOR_TYPE_GRAY_ALPHA

enum
{
    OPTION_VERBOSE = 1
};

char fileName[128];

void makeTest()
{
    printf("generating test image\n");

    char *ima = new char[256 * 64];

    for (int i = 0; i < 256 * 64; i++)
    {
        ima[i] = i / 64;
    }

    CFrame *images = new CFrame[1];
    images[0].m_nLen = images[0].m_nHei = 16 * 8;
    images[0].setRGB(new uint32_t[images[0].m_nLen * images[0].m_nHei]);
    char *bitmap = CFrameSet::ima2bitmap(ima, 16, 16);

    CFrameSet::bitmap2rgb(bitmap, images[0].getRGB(), images[0].m_nLen, images[0].m_nHei, 0);
    uint8_t *png;
    int totalSize;
    images[0].toPng(png, totalSize);

    CFileWrap tfile;
    if (tfile.open("test.png", "wb"))
    {
        tfile.write(png, totalSize);
    }
    tfile.close();

    delete[] png;
    delete[] ima;
    delete[] images;
}

enum
{
    CMD_TEST,
    CMD_MERGE,
    CMD_PNG,
    CMD_HELP,
    CMD_CONVERT,
    CMD_VERBOSE,
    CMD_SPLIT,
    CMD_OUTPATH,
    CMD_ENLARGE,
    CMD_TOP_PIX
};

int findCmd(char *cmd)
{
    static const char *list[] = {
        "-test",
        "-m",
        "-png",
        "-help",
        "-c",
        "-v",
        "-s",
        "-o",
        "-x",
        "-tp",
        nullptr};

    for (int i = 0; list[i]; i++)
    {
        if (strcmp(cmd, list[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

void showUsage()
{
    int ver[4];
    int version = SS_LGCK_VERSION;
    for (int i = 0; i < 4; ++i)
    {
        ver[i] = version & 0xff;
        version /= 256;
    }

    printf("Obl5Tool v%d.%d.%d.%d\n\n"
           "usage:\nobl5t [options...] <files>\n\n"
           "options\n"
           "-m[:fname]\tmerge all images into one file (.obl5)\n"
           "-c\t\tconvert individual files to obl5\n"
           "-png\t\toutput all images to png\n"
           "-v\t\textra verbose\n"
           "-s[:px]\t\tsplit at pixel (def: 16px)\n"
           "-o:path\t\tdefine the output directory\n"
           "-x\t\tenlarge resulting image by 2x\n"
           "-tp\t\tuse top pixel as transparent color\n"
           "\nThis program can only process: obl, obl5 and png files\n\n",
           ver[3], ver[2], ver[1], ver[0]);
}

int min(int a, int b)
{
    if (a > b)
    {
        return b;
    }
    else
    {
        return a;
    }
}

int main(int argc, char *argv[], char *envp[])
{
    CFileWrap sfile;
    srand(time(0));
    if (argc < 2)
    {
        printf("too few arguments\n\n");
        showUsage();
        return -1;
    }

    const int DEFAULT_PX_SIZE = 16;

    char *files[argc];
    int j = 0;
    int pxSize = DEFAULT_PX_SIZE;

    bool merge = false;
    bool wantPng = false;
    bool wantOBL5 = false;
    bool hasCmd = false;
    bool wantVerbose = false;
    bool wantSplit = false;
    bool wantEnlarge = false;
    bool wantTest = false;
    bool useTopPix = false;
    int options = 0;

    char *outPath = new char[3];
    strcpy(outPath, "./");
    char *mergeName = new char[10];
    strcpy(mergeName, "merge~%.4x");

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            if (1 == 1)
            {
                files[j] = argv[i];
                j++;
            }
        }
        else
        {

            char cmdString[strlen(argv[i])];
            strcpy(cmdString, argv[i]);
            char *p = strstr(cmdString, ":");
            if (p)
            {
                *p = 0;
            }

            int cmd = findCmd(cmdString);
            if (cmd != -1)
            {
                hasCmd = true;
            }
            switch (cmd)
            {
            case CMD_TEST:
                makeTest();
                wantTest = true;
                break;

            case CMD_MERGE:
                merge = true;

                if (p && p[1])
                {
                    delete[] mergeName;
                    int len = strlen(p + 1);
                    mergeName = new char[len + 2];
                    strcpy(mergeName, p + 1);
                }
                break;

            case CMD_PNG:
                wantPng = true;
                break;

            case CMD_HELP:
                showUsage();
                break;

            case CMD_CONVERT:
                wantOBL5 = true;
                break;

            case CMD_VERBOSE:
                wantVerbose = true;
                options |= OPTION_VERBOSE;
                break;

            case CMD_SPLIT:
                if (p)
                {
                    int v = strtol(p + 1, NULL, 10);
                    if (v && v % 8 == 0)
                    {
                        pxSize = v;
                    }
                    else
                    {
                        printf("pxSize must be a multiple of 8.");
                    }
                }
                wantSplit = true;
                break;

            case CMD_OUTPATH:
                if (p && p[1])
                {
                    delete[] outPath;
                    int len = strlen(p + 1);
                    outPath = new char[len + 2];
                    strcpy(outPath, p + 1);
                    if (p[len - 1] != '/' &&
                        p[len - 1] != '\\')
                    {
                        strcat(p, "/");
                    }
                }
                else
                {
                    printf("missing output path\n");
                }
                break;

            case CMD_ENLARGE:
                wantEnlarge = true;
                break;

            case CMD_TOP_PIX:
                useTopPix = true;
                break;

            default:
                printf("unknown command `%s`\n", argv[i]);
            }
        }
    }

    if (!wantPng && !wantOBL5 && !merge & !wantTest)
    {
        printf("please supply -png, -m or -c on command line\n\n");
        showUsage();
        return -1;
    }

    if (!j)
    {
        printf("no file to process\n\n");
        showUsage();
        return -1;
    }

    CFileWrap tfilem;
    int totalImages = 0;

    CFrameSet mergeSet;

    if (merge)
    {
        char t[16 + strlen(mergeName) + strlen(outPath)];
        sprintf(t, mergeName, rand());

        char mergefile[16 + strlen(mergeName) + strlen(outPath)];
        strcpy(mergefile, outPath);
        strcat(mergefile, t);
        strcat(mergefile, ".obl5");

        if (tfilem.open(mergefile, "wb"))
        {
        }
        else
        {
            printf("failed to create `%s`\n", mergefile);
        }
    }

    CFileWrap file;

    for (int n = 0; n < j; n++)
    {
        char *s = files[n];
        if (file.open(s, "rb"))
        {
            printf("file `%s` opened\n", s);
            strcpy(fileName, s);

            CFrameSet images;
            if (images.extract(file))
            {

                printf("images extracted: %d\n", images.getSize());

                if (useTopPix)
                {
                    for (int i = 0; i < images.getSize(); ++i)
                    {
                        images[i]->setTopPixelAsTranparency();
                    }
                }

                if (wantSplit)
                {
                    int size = images.getSize();
                    for (int i = 0; i < size; ++i)
                    {
                        CFrameSet *frameSet = images[i]->split(pxSize);
                        for (int n = 0; n < frameSet->getSize(); ++n)
                        {
                            images.add((*frameSet)[n]);
                        }
                        frameSet->removeAll();
                    }

                    for (int i = 0; i < size; ++i)
                    {
                        delete images[0];
                        images.removeAt(0);
                    }
                }

                for (int i = 0; i < images.getSize(); i++)
                {
                    if (wantEnlarge)
                    {
                        images[i]->enlarge();
                    }

                    if (wantPng)
                    {
                        if (i == 0)
                        {
                            printf("exporting to png...\n");
                        }

                        CFileWrap tfile;
                        char *tmp = new char[strlen(outPath) + strlen(s) + 16];
                        strcpy(tmp, outPath);
                        strcat(tmp, s);

                        char *lastSlash = strrchr(tmp, '/');
                        char *p = strrchr(tmp, '\\');
                        if (p > lastSlash)
                            lastSlash = p;

                        char *lastDot = strrchr(tmp, '.');
                        if (lastDot > lastSlash)
                        {
                            sprintf(lastDot, "~~%.4x.png", i);
                        }
                        else
                        {
                            sprintf(tmp + strlen(tmp), "~~%.4x.png", i);
                        }

                        uint8_t *png;
                        int totalSize;
                        images[i]->toPng(png, totalSize);

                        if (tfile.open(tmp, "wb"))
                        {
                            printf("  ==> %s\n", tmp);
                            tfile.write(png, totalSize);
                            tfile.close();
                        }
                        else
                        {
                            printf("failed to create `%s`\n", tmp);
                        }

                        delete[] tmp;
                        delete[] png;
                    }

                    if (merge)
                    {
                        CFrame *frame = new CFrame(images[i]);
                        mergeSet.add(frame);
                        totalImages++;
                    }
                }

                if (wantOBL5)
                {
                    printf("exporting to obl5...\n");

                    char *tmp = new char[strlen(outPath) + strlen(s) + 20];
                    strcpy(tmp, s);

                    char *lastSlash = strrchr(s, '/');
                    char *p = strrchr(s, '\\');
                    if (p > lastSlash)
                        lastSlash = p;

                    strcpy(tmp, outPath);
                    if (p)
                    {
                        strcat(tmp, p + 1);
                    }
                    else
                    {
                        strcat(tmp, s);
                    }

                    char *lastDot = strrchr(tmp, '.');
                    if (lastDot > lastSlash)
                    {
                        sprintf(lastDot, "~~%.4x.obl5", rand());
                    }
                    else
                    {
                        sprintf(tmp + strlen(tmp), "~~%.4x.obl5", rand());
                    }

                    CFileWrap tfile;
                    if (tfile.open(tmp, "wb"))
                    {
                        printf(" ==> %s\n", tmp);
                        images.write(tfile);
                        tfile.close();
                    }
                    else
                    {
                        printf("failed to create `%s`\n", tmp);
                    }

                    delete[] tmp;
                }
            }

            file.close();
        }
        else
        {
            printf("ERROR: can't decode file `%s`\n", s);
        }
    }

    delete[] outPath;
    delete[] mergeName;

    if (merge)
    {
        printf("total images merged: %d\n", totalImages);
        mergeSet.write(tfilem);
        tfilem.close();
    }
    return 0;
}
