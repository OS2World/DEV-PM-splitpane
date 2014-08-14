/* --------------------------------------------------------------------------
 axBase.h :

 23 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _AXBASE_H_
   #define _AXBASE_H_


/* --------------------------------------------------------------------------
 This macro calculates the size needed for a member of a variable size
 structure.
- Parameters -------------------------------------------------------------
 _struct_type_ : structure type.
 _member_      : member name.
- Return value -----------------------------------------------------------
 size_t : count of bytes needed for _member_
- Remarks ----------------------------------------------------------------
-------------------------------------------------------------------------- */
#define sizefor(_struct_type_, _member_)\
  (offsetof(_struct_type_, _member_) + sizeof(((_struct_type_*)0)->_member_))

// #define sizefor(_struct_type_, _member_) \
//    (size_t)(&((_struct_type_*)0)->_member_ \
//             + sizeof(((_struct_type_*)0)->_member_))


/* --------------------------------------------------------------------------
 This macro returns the count of elements of an array
- Parameters -------------------------------------------------------------
 _array_ : array variable.
- Return value -----------------------------------------------------------
 ULONG : count of array elements.
- Remarks ----------------------------------------------------------------
-------------------------------------------------------------------------- */
#define elementsof(_array_)  (sizeof(_array_) / sizeof(_array_[0]))


/* --------------------------------------------------------------------------
 This macro initialize the content of a structure to 0.
- Parameters -------------------------------------------------------------
 Structure variable.
-------------------------------------------------------------------------- */
#define zerostruct(_struct_) \
    memset(&(_struct_), 0, sizeof(_struct_))




#endif // #ifndef _AXBASE_H_