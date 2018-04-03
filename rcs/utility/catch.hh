#ifndef RCS_UTILITY_CATCH_HH__
#define RCS_UTILITY_CATCH_HH__

#include "../error.hh"

/**
 * @breif Catch a non-`error::none` `error` error code into a variable named 
 *   `err_var_name`, assigned from `err_returning_expr` with the handling scope 
 *   `on_fail_block`.
 *
\verbatim

```c++
 Usage:
 auto go() {
   // ...
   RCS_CATCH(err, something_risky(), {
     if (err == error::whatever)
       handle_whatever();
   }
   // `err` is now out of scope
 }
 ```

\endverbatim
 */
#define RCS_CATCH(err_var_name, err_returning_expr, on_fail_block) \
  do { \
    error err_var_name = err_returning_expr; \
    if (err_var_name != error::none) \
      on_fail_block; \
  } while (0)

#endif // RCS_UTILITY_CATCH_HH__
