#include "precomp.hxx"
#include <numeric>    // for iota

namespace hl
{

// 并查集实现
class UnionFind
{
public:
    std::vector<uint> parent;                     // 父节点数组
    std::vector<uint> rank;                       // 按秩合并的辅助数组

    UnionFind(uint n):
        parent(n), rank(n, 0)
    {
        iota(parent.begin(), parent.end(), 0);    // 初始化，每个元素指向自己
    }

    // 查找根节点（带路径压缩）
    int find(uint x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]);    // 路径压缩
        }
        return parent[x];
    }

    // 合并两个集合（按秩合并）
    void unite(uint x, uint y)
    {
        uint rootX = find(x);
        uint rootY = find(y);
        if (rootX != rootY)
        {
            if (rank[rootX] < rank[rootY])
            {
                parent[rootX] = rootY;
            }
            else if (rank[rootX] > rank[rootY])
            {
                parent[rootY] = rootX;
            }
            else
            {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }
};

void connectedComponents(const Mat& src, Mat& dst)
{
    // 创建一个空的标签矩阵，类型为32位整数（用于存储标签）
    Mat label              = Mat(src.size(), HL_32UC1, Scalar(0));

    int rows               = src.rows;
    int cols               = src.cols;

    uint      currentLabel = 1;    // 当前标签，从1开始
    UnionFind uf(rows * cols);     // 并查集：总共有 rows * cols 个可能的像素标签

    // 第一次扫描：初步分配标签并记录等价
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (src.at<uchar>(y, x) == 0) continue;    // 背景点跳过

            uint left                   = (x > 0) ? label.at<uint>(y, x - 1) : 0;
            uint leftUp                 = (y > 0 && x > 0) ? label.at<uint>(y - 1, x - 1) : 0;
            uint up                     = (y > 0) ? label.at<uint>(y - 1, x) : 0;
            uint rightUp                = (y > 0 && x < cols - 1) ? label.at<uint>(y - 1, x + 1) : 0;

            std::vector<uint> neighbors = {left, leftUp, up, rightUp};
            neighbors.erase(remove(neighbors.begin(), neighbors.end(), 0), neighbors.end());    // 删除背景点

            if (neighbors.empty())
            {
                // 没有邻接标记，分配新标签
                label.at<uint>(y, x) = currentLabel;
                currentLabel++;
            }
            else
            {
                // 存在标记，取最小标记
                uint minLabel        = *min_element(neighbors.begin(), neighbors.end());
                label.at<uint>(y, x) = minLabel;

                // 等价记录
                for (uint label : neighbors)
                {
                    uf.unite(minLabel, label);
                }
            }
        }
    }

    // 第二次扫描：统一标签
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (label.at<uint>(y, x) > 0)
            {
                // 找到当前像素的最终根标签
                label.at<uint>(y, x) = uf.find(label.at<uint>(y, x));
            }
        }
    }

    // 给dst涂色
    cvtColor(src, dst, COLOR_GRAY2BGR);

    std::vector<Vec3b> colors(currentLabel);

    for (uint i = 0; i < currentLabel; ++i)
    {
        colors[i] = Vec3b(rand() & 255, rand() & 255, rand() & 255);
    }

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (label.at<uint>(y, x) > 0)
            {
                dst.at<Vec3b>(y, x) = colors[label.at<uint>(y, x)];
            }
        }
    }
}

}    // namespace hl