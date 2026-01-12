#pragma once
#include <iomanip>
#include <iostream>

template <typename T = float> struct Matrix
{
    struct Column;
    struct Row;
    friend struct Column;
    friend struct Row;
    friend struct columnIterator;
    template <typename U> friend std::ostream& operator<<(std::ostream&, const Matrix<U>&);

  private:
    size_t rows_, cols_;
    std::vector<T> data;

  public:
    Matrix(size_t r, size_t c) : rows_(r), cols_(c), data(r * c, 0) {}

    Matrix(std::initializer_list<std::initializer_list<T>> init)
        : rows_(init.size()), cols_(init.size() > 0 ? init.begin()->size() : 0), data(rows_ * cols_)
    {
        size_t r = 0;
        for (const auto& row : init)
        {
            size_t c = 0;
            for (const auto& val : row)
            {
                data[r * cols_ + c] = val;
                c++;
            }
            r++;
        }
    }

    auto& operator()(size_t r, size_t c)
    {
        return data[r * cols_ + c];
    }
    const auto& operator()(size_t r, size_t c) const
    {
        return data[r * cols_ + c];
    }

    auto operator==(const Matrix<T>& other) const
    {
        return data == other.data;
    }

    auto& swapRows(size_t r1, size_t r2)
    {
        std::swap_ranges(row(r1).begin(), row(r1).end(), row(r2).begin());
        return *this;
    }

    auto& swapColumns(size_t c1, size_t c2)
    {
        std::swap_ranges(column(c1).begin(), column(c1).end(), column(c2).begin());
        return *this;
    }

    auto& scaleRow(size_t r, T s)
    {
        transform(row(r).begin(), row(r).end(), row(r).begin(), [s](T v) { return v * s; });
        return *this;
    }

    auto& addRows(size_t r1, size_t r2, T s)
    {
        transform(row(r1).begin(), row(r1).end(), row(r2).begin(), row(r2).begin(),
                  [s](T v1, T v2) { return v1 * s + v2; });
        return *this;
    }

    auto rows() const
    {
        return rows_;
    }
    auto cols() const
    {
        return cols_;
    }
    auto row(size_t r)
    {
        return Row{r, this};
    }
    auto column(size_t c)
    {
        return Column{c, this};
    }

    Matrix& rowReduce();

    struct ColumnIterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ssize_t;
        using pointer = T*;
        using reference = T&;
        friend struct Column;

      public:
        auto& operator*()
        {
            return (*matrix_)(r, c);
        }
        auto& operator[](difference_type i)
        {
            return (*matrix_)(r + i, c);
        }
        auto operator->()
        {
            return &(**this);
        }
        auto& operator++()
        {
            r++;
            return *this;
        }
        auto& operator--()
        {
            r--;
            return *this;
        }
        auto operator++(int)
        {
            ColumnIterator temp = *this;
            ++*this;
            return temp;
        }
        auto operator--(int)
        {
            ColumnIterator temp = *this;
            --*this;
            return temp;
        }
        auto operator==(const ColumnIterator& other) const
        {
            return r == other.r;
        }
        auto operator!=(const ColumnIterator& other) const
        {
            return !(*this == other);
        }
        auto operator<(const ColumnIterator& other) const
        {
            return r < other.r;
        }
        auto operator>(const ColumnIterator& other) const
        {
            return other < *this;
        }
        auto operator>=(const ColumnIterator& other) const
        {
            return !(*this < other);
        }
        auto operator<=(const ColumnIterator& other) const
        {
            return !(*this > other);
        }
        auto& operator+=(difference_type n)
        {
            r += n;
            return *this;
        }
        auto& operator-=(difference_type n)
        {
            r -= n;
            return *this;
        }
        auto operator+(difference_type n) const
        {
            return ColumnIterator{r + n, c, matrix_};
        }
        auto operator-(difference_type n) const
        {
            return ColumnIterator{r - n, c, matrix_};
        }
        auto operator-(const ColumnIterator& other) const
        {
            return static_cast<difference_type>(r) - static_cast<difference_type>(other.r);
        }
        friend ColumnIterator operator+(difference_type n, const ColumnIterator& it)
        {
            return it + n;
        }

      private:
        size_t r;
        size_t c;
        Matrix* matrix_;
        ColumnIterator(size_t r_, size_t c_, Matrix* m) : r(r_), c(c_), matrix_(m) {}
    };

    struct Column
    {
        size_t c;
        Matrix* matrix_;
        auto& operator[](size_t r)
        {
            return (*matrix_)(r, c);
        }
        auto begin()
        {
            return ColumnIterator{0, c, matrix_};
        }
        auto end()
        {
            return ColumnIterator{matrix_->rows_, c, matrix_};
        }
    };

    struct Row
    {
        size_t r;
        Matrix* matrix_;
        auto& operator[](size_t c)
        {
            return (*matrix_)(r, c);
        }
        auto begin()
        {
            return matrix_->data.begin() + r * matrix_->cols_;
        }
        auto end()
        {
            return matrix_->data.begin() + (r + 1) * matrix_->cols_;
        }
    };
};

template <typename T> std::ostream& operator<<(std::ostream& os, const Matrix<T>& m)
{
    for (size_t r = 0; r < m.rows_; r++)
    {
        for (size_t c = 0; c < m.cols_; c++)
        {
            // NOLINTNEXTLINE
            os << std::setprecision(1) << std::setw(5) << std::setfill(' ') << std::fixed << m(r, c)
               << " ";
        }
        os << std::endl;
    }
    return os;
}

template <typename T> Matrix<T>& Matrix<T>::rowReduce()
{
    size_t lead = 0;
    for (size_t r = 0; r < rows_; r++)
    {
        if (lead >= cols_)
        {
            return *this;
        }
        size_t i = r;
        while (data[i * cols_ + lead] == 0)
        {
            i++;
            if (i == rows_)
            {
                i = r;
                lead++;
                if (lead == cols_)
                {
                    return *this;
                }
            }
        }
        if (i != r)
        {
            swapRows(i, r);
        }

        T lv = data[r * cols_ + lead];
        if (lv != 1)
        {
            scaleRow(r, 1 / lv);
            // std::cout << *this << std::endl;
        }
        lv = data[r * cols_ + lead];
        for (size_t i = 0; i < rows_; i++)
        {
            if (i != r)
            {
                T lv2 = data[i * cols_ + lead];
                T factor = lv2 / lv;
                if (factor != 0)
                {
                    addRows(r, i, -lv2 / lv);
                    // std::cout << *this << std::endl;
                }
            }
        }
        lead++;
    }
    return *this;
}