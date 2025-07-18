/*******************************************************************************
  Copyright(c) 2024 QHY. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#include "qhy_mount_base.h"
#include <connectionplugins/connectionserial.h>

QHYMountBase::QHYMountBase() : EQMod()
{
    setTelescopeConnection(CONNECTION_SERIAL);
}

const char * QHYMountBase::getDefaultName()
{
    return "QHY Mount";
}

bool QHYMountBase::initProperties()
{
    EQMod::initProperties();
    for (auto oneProperty : *getProperties())
    {
        oneProperty.setDeviceName(getDeviceName());
    }

    return true;
}
