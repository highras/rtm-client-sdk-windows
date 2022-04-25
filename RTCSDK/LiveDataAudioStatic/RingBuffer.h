#pragma once

#include <stddef.h>

enum Wrap { SAME_WRAP, DIFF_WRAP };

template<class T>
class RingBuffer {
    size_t read_pos;
    size_t write_pos;
    size_t element_count;
    enum Wrap rw_wrap;
    T* data;

    size_t GetBufferReadRegions(size_t count, T** data_ptr_1, size_t* data_ptr_bytes_1, T** data_ptr_2, size_t* data_ptr_bytes_2) {

        const size_t readable_elements = AvailableRead();
        const size_t read_elements = (readable_elements < count ?
            readable_elements : count);
        const size_t margin = element_count - read_pos;

        // Check to see if read is not contiguous.
        if (read_elements > margin) {
            // Write data in two blocks that wrap the thisfer.
            *data_ptr_1 = data + read_pos;
            *data_ptr_bytes_1 = margin * sizeof(T);
            *data_ptr_2 = data;
            *data_ptr_bytes_2 = (read_elements - margin) * sizeof(T);
        }
        else {
            *data_ptr_1 = data + read_pos;
            *data_ptr_bytes_1 = read_elements * sizeof(T);
            *data_ptr_2 = NULL;
            *data_ptr_bytes_2 = 0;
        }

        return read_elements;
    }
public:
    RingBuffer(size_t ele_count) {
        data = new T[ele_count];
        element_count = ele_count;
        Reset();
    }

    void Reset() {
        read_pos = 0;
        write_pos = 0;
        rw_wrap = SAME_WRAP;

        // Initialize buffer to zeros
        memset(data, 0, element_count * sizeof(T));
    }

    ~RingBuffer() {
        delete[] data;
    }

    size_t Read(T* pT, size_t count) {
        T* buf_ptr_1 = NULL;
        T* buf_ptr_2 = NULL;
        size_t buf_ptr_bytes_1 = 0;
        size_t buf_ptr_bytes_2 = 0;
        const size_t read_count = GetBufferReadRegions(count, &buf_ptr_1, &buf_ptr_bytes_1, &buf_ptr_2, &buf_ptr_bytes_2);
        if (buf_ptr_bytes_2 > 0) {
            // We have a wrap around when reading the buffer. Copy the buffer data to
            // |data| and point to it.
            memcpy(pT, buf_ptr_1, buf_ptr_bytes_1);
            memcpy(((char*)pT) + buf_ptr_bytes_1, buf_ptr_2, buf_ptr_bytes_2);
        }
        else {
            // No wrap, but a memcpy was requested.
            memcpy(pT, buf_ptr_1, buf_ptr_bytes_1);
        }

        // Update read position
        MoveReadPrt((int)read_count);

        return read_count;
    }

    size_t Write(const T* pT, size_t count) {
        const size_t free_elements = AvailableWrite();
        const size_t write_elements = (free_elements < count ? free_elements
            : count);
        size_t n = write_elements;
        const size_t margin = this->element_count - this->write_pos;

        if (write_elements > margin) {
            // Buffer wrap around when writing.
            memcpy(this->data + this->write_pos, pT, margin * sizeof(T));
            this->write_pos = 0;
            n -= margin;
            this->rw_wrap = DIFF_WRAP;
        }
        memcpy(this->data + this->write_pos, pT + (write_elements - n), n * sizeof(T));
        this->write_pos += n;

        return write_elements;
    }

    size_t Write(const T t, size_t count) {
        const size_t free_elements = AvailableWrite();
        const size_t write_elements = (free_elements < count ? free_elements
            : count);
        size_t n = write_elements;
        const size_t margin = this->element_count - this->write_pos;

        if (write_elements > margin) {
            // Buffer wrap around when writing.
            memset(this->data + this->write_pos, t, margin * sizeof(T));
            this->write_pos = 0;
            n -= margin;
            this->rw_wrap = DIFF_WRAP;
        }
        memset(this->data + this->write_pos, t, n * sizeof(T));
        this->write_pos += n;

        return write_elements;
    }

    int MoveReadPrt(int count) {
        // We need to be able to take care of negative changes, hence use "int"
        // instead of "size_t".
        const int free_elements = (int)AvailableWrite();
        const int readable_elements = (int)AvailableRead();
        int read_pos = (int)this->read_pos;

        if (count > readable_elements) {
            count = readable_elements;
        }
        if (count < -free_elements) {
            count = -free_elements;
        }

        read_pos += count;
        if (read_pos > (int) this->element_count) {
            // Buffer wrap around. Restart read position and wrap indicator.
            read_pos -= (int)this->element_count;
            this->rw_wrap = SAME_WRAP;
        }
        if (read_pos < 0) {
            // Buffer wrap around. Restart read position and wrap indicator.
            read_pos += (int)this->element_count;
            this->rw_wrap = DIFF_WRAP;
        }

        this->read_pos = (size_t)read_pos;

        return count;
    }

    size_t AvailableRead() {
        if (this->rw_wrap == SAME_WRAP) {
            return this->write_pos - this->read_pos;
        }
        else {
            return this->element_count - this->read_pos + this->write_pos;
        }
    }

    size_t AvailableWrite() {
        return this->element_count - AvailableRead();
    }
};
