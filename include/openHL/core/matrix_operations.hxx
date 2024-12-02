#pragma once
#include <stdexcept>
#include <vector>
#include <initializer_list>
#include <cmath>

namespace hl {

template <typename T>
struct Matrix {
    int rows;
    int cols;
    std::vector<std::vector<T>> data;
    
    Matrix(int r, int c) : rows(r), cols(c), data(r, std::vector<T>(c, 0)) {}

    Matrix(std::initializer_list<std::initializer_list<T>> init) {
        rows = init.size();
        cols = init.begin()->size();
        data.resize(rows);
        int i = 0;
        for (const auto& row : init) {
            data[i++] = std::vector<T>(row);
        }
    }

    std::vector<T>& operator[](int i) {
        return data[i];
    }

    const std::vector<T>& operator[](int i) const {
        return data[i];
    }
};

// 矩阵乘法函数
template <typename T>
Matrix<T> multiply(const Matrix<T>& a, const Matrix<T>& b) {
    if (a.cols != b.rows) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
    }

    Matrix<T> result(a.rows, b.cols);
    for (int i = 0; i < a.rows; ++i) {
        for (int j = 0; j < b.cols; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < a.cols; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

// 高斯消元法计算矩阵的行列式
template <typename T>
T determinant(Matrix<T>& mat) {
    if (mat.rows != mat.cols) {
        throw std::invalid_argument("Matrix must be square to compute determinant.");
    }

    int n = mat.rows;
    T det = 1;

    for (int i = 0; i < n; ++i) {
        // 寻找主元
        int pivotRow = i;
        for (int j = i + 1; j < n; ++j) {
            if (std::abs(mat[j][i]) > std::abs(mat[pivotRow][i])) {
                pivotRow = j;
            }
        }

        if (mat[pivotRow][i] == 0) {
            return 0; // 行列式为零
        }

        if (pivotRow != i) {
            // 交换行
            std::swap(mat[i], mat[pivotRow]);
            det = -det; // 交换行，行列式符号改变
        }

        T pivot = mat[i][i];
        for (int j = i + 1; j < n; ++j) {
            T factor = mat[j][i] / pivot;
            for (int k = i; k < n; ++k) {
                mat[j][k] -= mat[i][k] * factor;
            }
        }
        det *= pivot;
    }
    return det;
}

// 高斯-约旦消元法计算矩阵的逆
template <typename T>
Matrix<T> inverse(Matrix<T>& mat) {
    if (mat.rows != mat.cols) {
        throw std::invalid_argument("Matrix must be square to compute inverse.");
    }

    int n = mat.rows;
    Matrix<T> augmented(n, 2 * n);

    // 创建增广矩阵 [mat | I]
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            augmented[i][j] = mat[i][j];
            augmented[i][j + n] = (i == j) ? 1 : 0;
        }
    }

    // 高斯-约旦消元
    for (int i = 0; i < n; ++i) {
        T pivot = augmented[i][i];
        if (pivot == 0) {
            throw std::runtime_error("Matrix is singular and cannot be inverted.");
        }

        for (int j = 0; j < 2 * n; ++j) {
            augmented[i][j] /= pivot;
        }

        for (int j = 0; j < n; ++j) {
            if (i != j) {
                T factor = augmented[j][i];
                for (int k = 0; k < 2 * n; ++k) {
                    augmented[j][k] -= augmented[i][k] * factor;
                }
            }
        }
    }

    // 提取逆矩阵
    Matrix<T> result(n, n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = augmented[i][j + n];
        }
    }

    return result;
}

} // namespace hl
