

namespace dev_dst
{   
    template <typename T>
    class vector {
        public:
            vector();
            vector(uint32_t capacity);
            vector(uint32_t capacity, T value);
            vector(vector& other);
            vector(vector&& other);

            void reserve(uint32_t capacity);
            void resize(uint32_t size);


        private:
            T* data;
            uint32_t size;
            uint32_t capacity;
    };
    
} // namespace dev_dst
