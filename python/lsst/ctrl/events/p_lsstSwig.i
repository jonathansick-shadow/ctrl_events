// -*- lsst-c++ -*-
#if defined(SWIGPYTHON)
/*
 * don't allow user to add attributes to C-defined classes; catches a variety of
 * typos when the user _thinks_ that they're setting a C-level object, but in
 * reality they're adding a new (and irrelevent) member to the class
 */
%pythonnondynamic;
#endif

%naturalvar;                            // use const reference typemaps

%include "cpointer.i"
%include "exception.i"
%include "std_list.i"
%include "std_map.i"
%include "std_string.i"
%include "std_set.i"
%include "std_vector.i"
%include "std_iostream.i"
%include "typemaps.i"

%include "carrays.i"

// N.b. these may interfere with the use of e.g. std_list.i for primitive types;
// you'll have to say e.g.
// %clear int &;
//    %template(listInt)   std::list<int>;
//    %template(mapIntInt) std::map<int,int>;
// %apply int &OUTPUT { int & };

%apply int &OUTPUT { int & };
%apply float &OUTPUT { float & };
%apply double &OUTPUT { double & };

%array_class(float, floatArray);
%array_class(double, doubleArray);

/******************************************************************************/

%{
#   include "lsst/pex/exceptions/Exception.h"
%}

/******************************************************************************/
/*
 * Don't expose the entire boost::shared_ptr to swig; it is complicated...
 */
namespace boost {
    template<class T>
    class shared_ptr {
    public:
        shared_ptr(T *);
        ~shared_ptr();
        T *operator->() const;
        int use_count() const;
        T *get() const;
    };
}

/************************************************************************************************************/
/*
 * Required forward declaration to use make_output_iterator --- swig bug?
 */
%{
    namespace swig {
        template<typename OutIter>
        PySwigIterator*
        make_output_iterator(const OutIter& current, const OutIter& begin,const OutIter& end, PyObject *seq);
    }
%}

/******************************************************************************/

%exception {
    try {
        $action
    } catch (lsst::pex::exceptions::ExceptionStack &e) {
        PyErr_SetString(PyExc_IndexError, e.what());
        return NULL;
    }
}

/******************************************************************************/
/*
 * Throw an exception if func returns NULL
 */
%define NOTNULL(func)
    %exception func {
        $action;
	if (result == NULL) {
	    $cleanup;
	}
    }
%enddef

/*
 * Throw an exception if func returns a negative value
 */
%define NOTNEGATIVE(func)
    %exception func {
        $action;
	if (result < 0) {
	    $cleanup;
	}
    }
%enddef

/******************************************************************************/

%define CAST(TYPE)
    %pointer_cast(void *, TYPE *, cast_ ## TYPE ## Ptr); // convert void pointer to (TYPE *)
%enddef

/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***
