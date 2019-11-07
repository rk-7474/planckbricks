// SPDX-License-Identifier: MIT
// Copyright (c) 2018 David Lechner

#include <pberror.h>

#include "py/mperrno.h"
#include "py/runtime.h"

/**
 * Raise an exception if *error* is not *PBIO_SUCCESS*. Most errors translate
 * to an OSError with the appropriate error code. There are a few special
 * cases that use another built-in python exception when it is more appropriate.
 */
void pb_assert(pbio_error_t error) {
    // using EINVAL to mean that the argument to this function was invalid.
    // since we raise ValueError for PBIO_ERROR_INVALID_ARG, there isn't a
    // possible conflict
    int os_err = MP_EINVAL;

    switch (error) {
    case PBIO_SUCCESS:
        return;
    case PBIO_ERROR_FAILED:
        mp_raise_msg(&mp_type_RuntimeError, pbio_error_str(error));
        return;
    case PBIO_ERROR_INVALID_ARG:
    case PBIO_ERROR_INVALID_PORT:
        mp_raise_ValueError(pbio_error_str(error));
        return;
    case PBIO_ERROR_NOT_IMPLEMENTED:
        mp_raise_NotImplementedError(pbio_error_str(error));
        return;
    case PBIO_ERROR_IO:
        os_err = MP_EIO;
        break;
    case PBIO_ERROR_NO_DEV:
        mp_raise_msg(&mp_type_OSError, pbio_error_str(error));
        return;
    case PBIO_ERROR_NOT_SUPPORTED:
        os_err = MP_EOPNOTSUPP;
        break;
    case PBIO_ERROR_AGAIN:
        os_err = MP_EAGAIN;
        break;
    case PBIO_ERROR_INVALID_OP:
        os_err = MP_EPERM;
        break;
    case PBIO_ERROR_TIMEDOUT:
#if MICROPY_PY_BUILTINS_TIMEOUTERROR
        mp_raise_msg(&mp_type_TimeoutError, NULL);
        return;
#else
        os_err = MP_ETIMEDOUT;
        break;
#endif
    case PBIO_ERROR_CANCELED:
        os_err = MP_ECANCELED;
        break;
    }

    mp_raise_OSError(os_err);
}
