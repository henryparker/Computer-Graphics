# Eigen类

## 向量 形如`Vector3f`

- 基本运算：$+,-,*$
- `vec1.dot(vec2)`：`vec1`与`vec2`点乘
- `vec1.cross(vec2)`：`vec1`与`vec2`叉乘
- `vec1.normalize()`：对`vec1`进行标准化
- `vec1 = vec2.normalized()`：对`vec1`进行`vec2`标准化的赋值
- `length = vec1.norm()`：求`vec1`的模长
- `vec2 = vec1.head<3>()`：将四维向量`vec1`前三维赋值`vec2`

## 矩阵 形如`Eigen::Matrix4f`

- 基本运算：$+,-$
- 矩阵乘法：$*$
- `matrix1 = matrix.transpose()`：矩阵转置
- `matrix1 = matrix.inverse()`：矩阵求逆
- `Matrix2f T{{0,1},{1,0}}`：矩阵构造