# :computer: C-Webserver-Project

## Description
In this project, I created webservers from the "ground-up" using C. In particular, these webservers can be accessed through one's browser and use two different methods to service users. The first is using threads to handle clients. The second method used was with the poll(2) system call which uses non-blocking io to prevent mishandlings of multiple clients.

## Purpose
The purpose of this project is to explore networking libraries in C and gain an understanding of how servers work from a super basic level. Through this, I hope to gain a better understanding of how frameworks such as Spring and Django work. This is not an exercise in building a useful or aesthetically pleasing website by any means. Rather, it is a proof of concept that I understand how servers and browsers interact and send information from one another at a low level of abstraction.

## Contents
### Servers:
* **threaded-webserver.c**: Webserver created using threads (posix thread library) that services basic HTML pages with GET and 404 errors if page isn't found
* **poll-webserver.c**: Webserver created using poll(2) that services basic HTML pages with GET and 404 errors if page isn't found
* **post-webserver.c**: Webserver created using poll(2) that services HTML, CSS, JavaScript, and JSON files with GET, POST, and 404 errors. Server sends JSON objects in a RESTful way, allowing for users to save information on server using POST.

## Building
1. Clone repository
2. Pull my custom parsing library, [Request-Parse](https://github.com/dborah123/Request-Parse), into project folder
3. Create a new folder in project folder called "cjSON" and copy and paste files "cJSON.c" and "cJSON.h" from [cJSON](https://github.com/DaveGamble/cJSON) following correct usage
4. Initiate make command into terminal to compile project
5. Use syntax: "./[server executable] [port #]" to host on a localhost
6. Go to a browser and use urls to test out the site:
    * 127.0.0.1:[port #]/home
    * 127.0.0.1:[port #]/about
    * 127.0.0.1:[port #]/sign-up
