


#include <iostream>
#include <vector>


void sort(std::vector<int>& vec) {
    std::sort(std::begin(vec), std::end(vec));
}

// bubble sort
void bubble_sort(std::vector<int>& vec) {
    for (int j = 0; j < vec.size() - 1; ++j) {
        for (int i = 0; i < vec.size() - 1; ++i) {
            if (vec[i] > vec[i+1]) {
                std::swap(vec[i], vec[i+1]);
            }
        }
    }
}


// insertion sort
void insertion_sort(std::vector<int>& vec) {
    for (int i = 0; i < vec.size(); ++i) {
        int min_val = vec[i];
        int min_index = i;
        for (int j = i; j < vec.size(); ++j) {
            if (vec[j] < min_val) {
                min_val = vec[j];
                min_index = j;
            }
        }
        std::swap(vec[i], vec[min_index]);
    }
}


// merge sort
std::vector<int> _merge_sort(std::vector<int>& vec, int l, int r) {
    if (l >= r) return std::vector<int>();
    if (l == r - 1) return std::vector<int>(1, vec[l]);

    int m = ((r - l) / 2) + l;

    std::vector<int> l_vec = _merge_sort(vec, l, m);
    std::vector<int> r_vec = _merge_sort(vec, m, r);

    std::vector<int> tmp;
    tmp.reserve(r-l);
    int i = 0, j = 0;
    while (i < l_vec.size() && j < r_vec.size()) {
        if (l_vec[i] <= r_vec[j]) {
            tmp.push_back(l_vec[i++]);
        } else {
            tmp.push_back(r_vec[j++]);
        }
    }

    tmp.insert(tmp.end(), l_vec.begin() + i, l_vec.end());
    tmp.insert(tmp.end(), r_vec.begin() + j, r_vec.end());

    return tmp;
    
}
void merge_sort(std::vector<int>& vec) {
    vec = std::move(_merge_sort(vec, 0, vec.size()));
}


int main()
{
    std::vector<int> vec({1, 4, 5, 6, 2, 23, 4, 5, 6});

    std::vector<int> vec2;

    vec2 = vec;
    sort(vec2);
    std::cout << "Testing sort: ";
    std::copy(begin(vec2), end(vec2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    vec2 = vec;
    bubble_sort(vec2);
    std::cout << "Bubble sort: ";
    std::copy(begin(vec2), end(vec2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    vec2 = vec;
    insertion_sort(vec2);
    std::cout << "Insertion sort: ";
    std::copy(begin(vec2), end(vec2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    vec2 = vec;
    merge_sort(vec2);
    std::cout << "Merge sort: ";
    std::copy(begin(vec2), end(vec2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;


}