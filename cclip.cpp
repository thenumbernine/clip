#include "cclip.h"
#include "clip.h"

static_assert(std::is_same<clip::format, ClipFormat>::value, "clip::format doesn't match");

//these are different types even if they are identical ...
//static_assert(std::is_same<clip::error_handler, ClipErrorHandler>::value, "clip::error_handler doesn't match");

// can I 'forward declare' a struct in a C header, then 'using' it later in a C++ file?
// kind of like type prototyping ... would be useful instead of a bunch of static-casts ...
//sadly no ...

static_assert(sizeof(clip::image_spec) == sizeof(ClipImageSpec), "clip::image_spec and ClipImageSpec size doesn't match");
#define ASSERT_FIELDS_MATCH(FIELD)\
static_assert(sizeof(((clip::image_spec*)nullptr)->FIELD) == sizeof(((ClipImageSpec*)nullptr)->FIELD), "clip::image_spec and ClipImageSpec field " #FIELD " size doesn't match");\
static_assert(offsetof(clip::image_spec, FIELD) == offsetof(ClipImageSpec, FIELD), "clip::image_spec and ClipImageSpec field " #FIELD " offset doesn't match");
ASSERT_FIELDS_MATCH(width)
ASSERT_FIELDS_MATCH(height)
ASSERT_FIELDS_MATCH(bits_per_pixel)
ASSERT_FIELDS_MATCH(bytes_per_row)
ASSERT_FIELDS_MATCH(red_mask)
ASSERT_FIELDS_MATCH(green_mask)
ASSERT_FIELDS_MATCH(blue_mask)
ASSERT_FIELDS_MATCH(alpha_mask)
ASSERT_FIELDS_MATCH(red_shift)
ASSERT_FIELDS_MATCH(green_shift)
ASSERT_FIELDS_MATCH(blue_shift)
ASSERT_FIELDS_MATCH(alpha_shift)

extern "C" {

ClipLock * clip_lock_new() {
	return (ClipLock*)new clip::lock();
}

ClipLock * clip_lock_new_p(void * native_window_handle) {
	return (ClipLock*)new clip::lock(native_window_handle);
}

void clip_lock_free(ClipLock * lock) { 
	delete (clip::lock*)lock;
}

bool clip_lock_locked(ClipLock const * lock) { 
	return ((clip::lock const *)lock)->locked(); 
} 

bool clip_lock_clear(ClipLock * lock) { 
	return ((clip::lock *)lock)->clear(); 
}

bool clip_lock_is_convertible(ClipLock const * lock, ClipFormat f) {
	return ((clip::lock const *)lock)->is_convertible(f);
}

bool clip_lock_set_data(ClipLock * lock, ClipFormat f, char const * buf, size_t len) {
	return ((clip::lock *)lock)->set_data(f, buf, len);
}

bool clip_lock_get_data(ClipLock const * lock, ClipFormat f, char * buf, size_t len) {
	return ((clip::lock const *)lock)->get_data(f, buf, len);
}

size_t clip_lock_get_data_length(ClipLock const * lock, ClipFormat f) {
	return ((clip::lock const *)lock)->get_data_length(f);
}

#if CLIP_ENABLE_IMAGE
bool clip_lock_set_image(ClipLock * lock, ClipImage const * image) {
	return ((clip::lock *)lock)->set_image(*(clip::image const *)image);
}

bool clip_lock_get_image(ClipLock const * lock, ClipImage * image) {
	return ((clip::lock const *)lock)->get_image(*(clip::image *)image);
}

bool clip_lock_get_image_spec(ClipLock const * lock, ClipImageSpec * spec) {
	return ((clip::lock const *)lock)->get_image_spec(*(clip::image_spec*)spec);
}
#endif
#if CLIP_ENABLE_LIST_FORMATS
#error TODO you are here, make a vector-getter for C
#endif

ClipFormat clip_register_format(char const * name, size_t len) {
	return clip::register_format(std::string(name, len));
}

ClipFormat clip_empty_format() {
	return clip::empty_format();
}

ClipFormat clip_text_format() {
	return clip::text_format();
}

#if CLIP_ENABLE_IMAGE
ClipFormat clip_image_format() {
	return clip::image_format();
}
#endif

bool clip_has(ClipFormat f) {
	return clip::has(f);
}

bool clip_clear() {
	return clip::clear();
}

void clip_set_error_handler(ClipErrorHandler f) {
	clip::set_error_handler((clip::error_handler)f);
}

ClipErrorHandler clip_get_error_handler() {
	return (ClipErrorHandler)clip::get_error_handler();
}

bool clip_set_text(char const * value, size_t len) {
	return clip::set_text(std::string(value,len));
}

bool clip_get_text(char * valuecptr, size_t * len) {
	std::string value;
	if (!clip::get_text(value)) return false;
	if (len) {
		// TODO +1 for null term?
		*len = value.size();
	}
	if (valuecptr) {
		memcpy(
			valuecptr,
			value.c_str(),
			// should I allow the user to specify a buffer size?
			// if so, then should I increment the 'len' upon reading above to include +1 for string length? (is this what glGetProgramInfoLog GL_INFO_LOG_LENGTH requires?)
			len ? std::max(*len, value.size()+1) : 
			value.size()+1
		);
	}
	return true;
}

#if CLIP_ENABLE_IMAGE
ClipImage * clip_image_new() {
	return (ClipImage*)new clip::image();
}

ClipImage * clip_image_new_p(ClipImageSpec const * spec) {
	return (ClipImage*)new clip::image(*(clip::image_spec const *)spec);
}

ClipImage * clip_image_new_pp(void const * data, ClipImageSpec const * spec) {
	return (ClipImage*)new clip::image(data, *(clip::image_spec const *)spec);
}

ClipImage * clip_image_new_fromImage(ClipImage const * image) {
	// TODO will this trigger the ref ctor or the move ctor?  
	// when it shouldn't be moved ...
	return (ClipImage*)new clip::image(*(clip::image const *)image);
}

void clip_image_free(ClipImage * image) {
	delete (clip::image*)image;
}

char * clip_image_data(ClipImage const * image) {
	return ((clip::image const *)image)->data();
}

ClipImageSpec const * clip_image_spec(ClipImage const * image) {
	return (ClipImageSpec const *)&((clip::image const *)image)->spec();
}

bool clip_image_is_valid(ClipImage const * image) {
	return ((clip::image const *)image)->is_valid();
}

void clip_image_reset(ClipImage * image) {
	((clip::image*)image)->reset();
}

bool clip_set_image(ClipImage const * image) {
	return clip::set_image(*(clip::image const *)image);
}

bool clip_get_image(ClipImage * image) {
	return clip::get_image(*(clip::image*)image);
}

bool clip_get_image_spec(ClipImageSpec * spec) {
	return clip::get_image_spec(*(clip::image_spec*)spec);
}
#endif

void clip_set_x11_wait_timeout(int msecs) {
	clip::set_x11_wait_timeout(msecs);
}

int clip_get_x11_wait_timeout() {
	return clip::get_x11_wait_timeout();
}

} // extern "C"
