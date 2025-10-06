


#include <iostream>
#include <vector>
#include <random>
#include <chrono>


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

// 2 3

void _quick_sort(std::vector<int>& vec, int l, int r){
    if (l >= r) return;
    int pivot = vec[r];
    int p = l; // points to last elt smaller
    for (int i = l; i < r; ++i) {
        if (vec[i] < vec[r]) {
            std::swap(vec[p++], vec[i]);
        }
    }
    std::swap(vec[p], vec[r]);
    _quick_sort(vec, l, p-1);
    _quick_sort(vec, p+1, r);
}

// quick_sort
void quick_sort(std::vector<int>& vec) {
    // find the pivot
    _quick_sort(vec, 0, vec.size()-1);
}

// faster merge sort

static void merge_into(std::vector<int>& a, std::vector<int>& buf, int l, int m, int r) {
    int i = l, j = m, k = l;
    while (i < m && j < r) buf[k++] = (a[i] <= a[j] ? a[i++] : a[j++]);
    while (i < m) buf[k++] = a[i++];
    while (j < r) buf[k++] = a[j++];
    for (int t = l; t < r; ++t) a[t] = buf[t];
}

static void mergesort(std::vector<int>& a, std::vector<int>& buf, int l, int r) {
    if (r - l <= 1) return;
    int m = l + (r - l) / 2;
    mergesort(a, buf, l, m);
    mergesort(a, buf, m, r);
    merge_into(a, buf, l, m, r);
}

void merge_sort_2(std::vector<int>& a) {
    if (a.size() < 2) return;
    std::vector<int> buf(a.size());
    mergesort(a, buf, 0, (int)a.size());
}

int main()
{
    // using namespace std::chrono;
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

    vec2 = vec;
    quick_sort(vec2);
    std::cout << "Quick sort: ";
    std::copy(begin(vec2), end(vec2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;



    std::random_device rd;
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dist(1,100);
    std::vector<int> nums(10000);
    for (auto &n : nums) n = dist(gen);
    


    
    vec2 = nums;
    auto t_start = std::chrono::high_resolution_clock::now();
    sort(vec2);
    auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Testing sort: ";
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;

    vec2 = nums;
    t_start = std::chrono::high_resolution_clock::now();
    bubble_sort(vec2);
    t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Bubble sort: ";
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;


    vec2 = nums;
    t_start = std::chrono::high_resolution_clock::now();
    insertion_sort(vec2);
    t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Insertion sort: ";
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;


    vec2 = nums;
    t_start = std::chrono::high_resolution_clock::now();
    merge_sort(vec2);
    t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Merge sort: ";
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;

    vec2 = nums;
    t_start = std::chrono::high_resolution_clock::now();
    merge_sort_2(vec2);
    t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Merge sort 2: ";
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;


    vec2 = nums;
    t_start = std::chrono::high_resolution_clock::now();
    quick_sort(vec2);
    t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Quick sort: ";
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Time taken: " << duration.count() << " ms\n";
    std::cout << std::endl;



}