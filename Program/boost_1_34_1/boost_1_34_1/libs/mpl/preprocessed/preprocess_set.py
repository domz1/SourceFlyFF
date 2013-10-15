
# Copyright Aleksey Gurtovoy 2001-2006
#
# Distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
#
# See http://www.boost.org/libs/mpl for documentation.

# $Source: /cvsroot/boost/boost/libs/mpl/preprocessed/preprocess_set.py,v $
# $Date: 2006/11/23 19:57:11 $
# $Revision: 1.2.8.1 $

import preprocess
import os.path

preprocess.main(
      [ "plain" ]
    , "set"
    , os.path.join( "boost", "mpl", "set", "aux_", "preprocessed" )
    )
