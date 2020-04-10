#pragma once

#include <epoxy/gl.h>
#include <cassert>
#include <memory>
#include <numeric>
#include <type_traits>
#include <window.h>

template <GLenum type, typename Ptr = void *>
struct buffer {
	friend void swap(buffer &a, buffer &b) {
		using std::swap;

		swap(a._id, b._id);
		swap(a._size, b._size);
		swap(a._buffer_ptr, b._buffer_ptr);
		swap(a._usage, b._usage);
	}

	buffer()
	:_id{0}, _size{0}, _buffer_ptr{nullptr} {}

	~buffer() {
		if (_buffer_ptr)
			unmap();

		if (_id)
			glDeleteBuffers(1, &_id);
	}

	buffer(const buffer &other) = delete;
	buffer(buffer &&other) {
		swap(*this, other);
	}

	buffer &operator=(const buffer &other) = delete;
	buffer &operator=(buffer &&other) {
		swap(*this, other);
		return *this;
	}

	void bind() {
		assert(_id);
		glBindBuffer(type, _id);
	}

	void unbind() {
		glBindBuffer(type, 0);
	}

	void generate() {
		glGenBuffers(1, &_id);
	}

	Ptr map() {
		bind();
		_buffer_ptr = static_cast<Ptr>(glMapBuffer(type, GL_READ_WRITE));
		unbind();

		assert(_buffer_ptr);
		return _buffer_ptr;
	}

	void store(Ptr data, size_t offset, size_t size) {
		if (_size < (size + offset)) {
			window::report_fatal("buffer::store: buffer overrun, buffer size = %lu, store size = %lu, store offset = %lu", _size, size, offset);
			abort();
		}
		bind();
		glBufferSubData(type, offset, size, data);
		unbind();
	}

	void unmap() {
		bind();
		glUnmapBuffer(type);
		unbind();

		_buffer_ptr = nullptr;
	}

	size_t size() const {
		return _size;
	}

	Ptr mapped_buffer() {
		return _buffer_ptr;
	}

	void store_regenerate(Ptr data, size_t size, GLenum usage) {
		if (_size == size) {
			store(data, 0, size);
			return;
		}

		_size = size;

		bind();
		glBufferData(type, _size, data, usage);
		unbind();
	}

	GLuint id() const {
		return _id;
	}

private:
	GLuint _id;
	size_t _size;
	Ptr _buffer_ptr;
};
