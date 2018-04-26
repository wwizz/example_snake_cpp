/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_EXAMPLE_CONSOLE_SEQUENCEMENU_H_
#define HUESTREAM_EXAMPLE_CONSOLE_SEQUENCEMENU_H_

#include <huestream_example_console/Menu.h>

class SequenceMenu : public Menu {
public:
    explicit SequenceMenu(const huestream::HueStreamPtr &hueStream);
    void Run();
};

#endif  // HUESTREAM_EXAMPLE_CONSOLE_SEQUENCEMENU_H_
