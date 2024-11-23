#include "precomp.h"

static void setSize(Mat* m, int row, int col)
{
    size_t esz = HL_ELEM_SIZE(m->flags);
    m->rows    = row;
    m->cols    = col;
    m->step[0] = col * esz;
    m->step[1] = esz;
}

void create(Mat* m, int row, int col, int type)
{
    type     = HL_MAT_TYPE(type);
    m->flags = type & HL_MAT_TYPE_MASK;
    setSize(m, row, col);
    if (total(m) > 0)
    {
        m->data = (unsigned char*)malloc((m->rows * m->step[0]) * sizeof(char));
    }
}

void copyTo(const Mat* m, Mat* out)
{
    *out      = *m;
    out->data = (unsigned char*)malloc(m->rows * m->step[0]);
    memcpy(out->data, m->data, m->rows * m->step[0]);
}

void release(Mat* m)
{
    free(m->data);
    m->data = NULL;
}

size_t elemSize(const Mat* m)
{
    return m->step[1];
}

size_t elemSize1(const Mat* m)
{
    return HL_ELEM_SIZE1(m->flags);
}

int type(const Mat* m)
{
    return HL_MAT_TYPE(m->flags);
}

int depth(const Mat* m)
{
    return HL_MAT_DEPTH(m->flags);
}

int channels(const Mat* m)
{
    return HL_MAT_CN(m->flags);
}

size_t step1(const Mat* m, int i)
{
    return m->step[i] / elemSize1(m);
}

bool empty(const Mat* m)
{
    return m->data == NULL || total(m) == 0;
}

size_t total(const Mat* m)
{
    return (size_t)m->rows * m->cols;
}