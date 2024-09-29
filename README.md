<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
<!-- [![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url] -->



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <!-- <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

  <h3 align="center">TsMAIN</h3>

  <p align="center">
    Monitoring and simulating different Bus messages basing on C++
    <br />
    <a href="https://github.com/1482815025/TsMAIN"><strong>Explore the docs »</strong></a>
    <br />
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

A C/C++ program that can simulate CAN/CANFD/LIN signals and send those messages on real bus. Mainly there are two libs needed, one is [db_Parser](https://github.com/1482815025/db_Parser), which can parse dbc/ldf files and provide decode and encode functions. The other one is [TsAPI](https://github.com/1482815025/TsAPI), which can send real time messages using Vector's hardware interfaces. Besides, there is another lib using to combine TsAPI and db_Parser, and implement real time Cntr & Chks of sending messages together with some real time message display & signals input functions etc. 

Functions available:
* Parse the database file(dbc & ldf files), simulate ECU node.
* Send messages on different Vector's hardware interfaces(CAN/CANFD/LIN/DAIO).
* Real-time counter & cheksum.
* Pre-defined config file to pass some valuables(overwrite some dbc default signal values during simulation)
* Availablility to change simulated signal's value at any time.
* Real-time decode(real-time signal monitoring).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

I personly use VSCode + CMAKE + MinGW64(x86_64-12.2.0-release-posix-seh-ucrt-rt_v10-rev2), and you can use my CMakeLists.txt to easily build this project.

* ![C++][C++-icon]
* [![CMake][CMake-icon]][CMake-url]
* [![MinGW][MinGW-icon]][MinGW-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.

### Prerequisites

* MinGW-w64
* CMake
* VSCode + CMake extensions
* libTsAPI.a, libdb_Parser.a, libSendFunc.a, vxlapi64

### Installation

After you have prepared the environment, you just need to clone the repo and start to build. Make sure you got libTsAPI.a, libdb_Parser.a, libSendFunc.a, vxlapi64.lib under <$workforder>/lib/static.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage

* You can design your own program with those APIs, here is an example about how to use config.ini file for pre-defined parameters inputs.

### ECU simulation and bus monitoring

```c++
#include "sendFunctions.hpp"
#include "readConfig.hpp"

// global variables
DbcParser dbcFile;
SendFunc* monitorCAN = new SendFunc();
std::thread sendThreadCAN;
std::thread inputThread;
Payloads_CAN encodedPayloadsCAN;
std::shared_ptr<CANFD> canfd;
std::shared_ptr<CAN> can;
Config test_config;

int main() {
  if (!readIniValue(test_config)) {
    std::cerr << "Error reading config.ini file." << std::endl;
    return 1;
  }

  if (!test_config.dbc_file.empty()) {
    // parse the dbcfile
    dbcFile.parse(test_config.dbc_file);
    monitorCAN->setDB(&dbcFile);
    if (dbcFile.databaseBusType == BusType::CAN_FD) {
      if (!test_config.dbc_node.empty()) {
        // generate Node signals default value
        dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
        if (!test_config.can_signals.empty()) {
          // apply overwrite signal values in config.ini
          for (auto& signal : test_config.can_signals) {
            dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
          }
        }
      }
      // initialize hardware, go on bus
      canfd = std::make_shared<CANFD>(dbcFile.Baudrate, dbcFile.BaudRateCANFD);
      // enable real time decode
      canfd->attach(monitorCAN);
      // start Node simulation
      sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd, std::ref(encodedPayloadsCAN));
    }
    else if (dbcFile.databaseBusType == BusType::CAN) {
      if (!test_config.dbc_node.empty()) {
        dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
        if (!test_config.can_signals.empty()) {
          for (auto& signal : test_config.can_signals) {
            dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
          }
        }
      }
      can = std::make_shared<CAN>(dbcFile.Baudrate);
      can->attach(monitorCAN);
      sendThreadCAN = std::thread(SendCANEncodedPayloadsThread, can, std::ref(encodedPayloadsCAN));
    }
    else {
      std::cout << "Invalid bus type." << std::endl;
      return 1;
    }
  }
  else {
    std::cout << "No dbc file provided!" << std::endl;
    return 1;
  }

  // enable cmd interactive function
  if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN_FD) {
    inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CANFD>>, canfd, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
  }
  else if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN) {
    inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CAN>>, can, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
  }

  if (sendThreadCAN.joinable()) {
    sendThreadCAN.join();
  }
  if (inputThread.joinable()) {
    inputThread.join();
  }
  system("pause");
  return 0;
}
```


_For more examples, please refer to the [Documentation](https://github.com/1482815025/TsMAIN/blob/master/README.md)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] CAN, CANFD, LIN support
- [x] Real time ECU simulation (Cntr, Chks)
- [x] Real time payload decode
- [x] Pre-defines paramters in config.ini
- [x] Support cmd input to change signal values
- [ ] Logging & replay
- [ ] User interface

<!-- See the [open issues](https://github.com/othneildrew/Best-README-Template/issues) for a full list of proposed features (and known issues). -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
<!-- ## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Top contributors:

<a href="https://github.com/othneildrew/Best-README-Template/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=othneildrew/Best-README-Template" alt="contrib.rocks image" />
</a>

<p align="right">(<a href="#readme-top">back to top</a>)</p> -->



<!-- LICENSE -->
## License

Distributed under the Apache-2.0 License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
<!-- ## Contact

Your Name - [@your_twitter](https://twitter.com/your_username) - email@example.com

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)

<p align="right">(<a href="#readme-top">back to top</a>)</p> -->



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments


* [TsAPI](https://github.com/1482815025/TsAPI)
* [db_Parser](https://github.com/1482815025/db_Parser)
* [SendFunc](https://github.com/1482815025/SendFunc)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[CMake-url]: https://cmake.org/
[CMake-icon]: https://img.shields.io/badge/CMake-000000?style=for-the-badge&logo=cmake
[C++-icon]: https://img.shields.io/badge/C++-000000?style=for-the-badge&logo=cplusplus
[MinGW-url]: https://www.mingw-w64.org/
[MinGW-icon]: https://img.shields.io/badge/MinGW64-000000?style=for-the-badge&logo=mingww64
