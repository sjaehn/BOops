/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef RINGBUFFER_HPP_
#define RINGBUFFER_HPP_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <new>
#include <algorithm>

template <class T>
class RingBuffer
{
public:
        RingBuffer ();
        RingBuffer (const size_t size);
        RingBuffer (const RingBuffer& that);
        ~RingBuffer ();

        RingBuffer& operator= (const RingBuffer& that);
        T& operator[] (const long n);
        const T& operator[] (const long n) const;
        size_t size () const;
        void resize (const size_t size);
        void fill (const T& value);
        void push_front (const T& value);
        void push_front (const T* values, const size_t n);
        void pop_front ();
        void pop_front (const size_t n);
        void move (const long n);

protected:
        T* data_;
        size_t size_;
        size_t position_;
};

template <class T> RingBuffer<T>::RingBuffer () : RingBuffer (0) {}

template <class T> RingBuffer<T>::RingBuffer  (const size_t size) : data_ (nullptr), size_ (0), position_ (0)
{
	if (size != 0)
	{
		try {resize (size);}
		catch (std::bad_alloc& ba) {throw ba;}
	}
}

template <class T> RingBuffer<T>::RingBuffer (const RingBuffer& that) : data_ (nullptr), size_ (0), position_ (0)
{
        if (that.size_ != 0)
        {
                try {resize (that.size_);}
                catch (std::bad_alloc& ba) {throw ba;}

                if ((data_) && (that.data_))
                {
                        std::copy (that.data_, that.data_ + size_, data_);
                        position_ = that.position_;
                }
        }
}

template <class T> RingBuffer<T>::~RingBuffer ()
{
	if (data_) delete[] (data_);
}

template <class T> const T& RingBuffer<T>::operator[] (const long n) const
{
        return data_[(position_ + size_ - 1 - (n % size_)) % size_];
}

template <class T> T& RingBuffer<T>::operator[] (const long n)
{
        return data_[(position_ + size_ - 1 - (n % size_)) % size_];
}

template <class T> RingBuffer<T>& RingBuffer<T>::operator= (const RingBuffer& that)
{
        try {resize (that.size_);}
        catch (std::bad_alloc& ba) {throw ba;}

        if ((data_) && (that.data_))
        {
                std::copy (that.data_, that.data_ + size_, data_);
                position_ = that.position_;
        }

        return *this;
}

template <class T> size_t RingBuffer<T>::size () const {return size_;}

template <class T> void RingBuffer<T>::resize (const size_t size)
{
        if (size == size_) return;

        T* newData = nullptr;
        if (size)
        {
                try {newData = new T[size];}
                catch (std::bad_alloc& ba)
                {
                        fprintf(stderr, "bad alloc\n");
                        throw ba;
                }

                std::fill (newData, newData + size, T());

                if (size >= size_)
                {
                        for (size_t i = 0; i < size_; ++i) newData[i] = data_[(position_ + i) % size_];
                        position_ = (size > 0 ? size_ % size : 0);
                }

                else
                {
                        for (size_t i = 0; i < size; ++i) newData[i] = data_[(position_ + size_ - size + i) % size_];
                        position_ = 0;
                }
        }

	if (data_) delete[] (data_);
        data_ = newData;
        size_ = size;
}

template <class T> void RingBuffer<T>::fill (const T& value)
{
        std::fill (data_, data_ + size_, value);
        position_ = 0;
}

template <class T> void RingBuffer<T>::push_front (const T& value)
{
        if (size_)
        {
                data_[position_] = value;
                position_ = (position_ + 1) % size_;
        }
}

template <class T> void RingBuffer<T>::push_front (const T* values, const size_t n)
{
        if (size_)
        {
                size_t nx = (n > size_ ? size_ : n);
                size_t n0 = n - nx;
                size_t i1 = (position_ + nx > size_ ? size_ - position_ : nx);
                for (size_t i = 0; i < i1; ++i) data_[position_ + i] = values[i + n0];
                for (size_t i = i1; i < nx; ++i) data_[i - i1] = values[i + n0];
                position_ = (position_ + n) % size_;
        }
}

template <class T>void RingBuffer<T>::pop_front ()
{
        if (size_) position_ = (position_ + size_ - 1) % size_;
}

template <class T>void RingBuffer<T>::pop_front (const size_t n)
{
        if (size_) position_ = (position_ + size_ - (n % size_)) % size_;
}

template <class T>void RingBuffer<T>::move (const long n)
{
        if (size_) position_ = (position_ + size_ - (n % size_)) % size_;
}

#endif /* RINGBUFFER_HPP_ */
