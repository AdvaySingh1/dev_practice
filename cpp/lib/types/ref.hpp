namespace dev_std {

    /**
     * @brief Allows for the forwarding of references
     * without expensive copies. See Barriers for sample
     * use
     * 
     */
    template <typename T>
    class reference {
        private:
            T* ptr; 
        public:
            explicit reference(T& obj) noexcept: ptr(&obj) {}
            operator T&() () { return *ptr; } // conversion operator
            T& get() const noexcept { return *ptr; }

    };

    template <typename T>
    reference<T> ref(T& t) noexcept { return reference<T>(T); }

    template <typename T>
    reference<const T> cref(const T& t) noexcept { return reference<const T>(T); }
}