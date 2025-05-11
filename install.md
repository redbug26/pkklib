
# User Guide for Installing `pkklib`

## Prerequisites

- [Docker](https://www.docker.com/) installed and functional
- [Visual Studio Code](https://code.visualstudio.com/) with the **Dev Containers** extension by Microsoft

## Installation Steps

### 1. Clone the GitHub Repository

Create a working directory dedicated to your `pkklib` projects. For example:

```bash
mkdir -p ~/dev/pkklib-projects
cd ~/dev/pkklib-projects
```

Then clone the repository into this directory:

```bash
git clone https://github.com/redbug26/pkklib
cd pkklib
```

### 2. Build the Docker Image

From the root of the PiKoKalc repository, build the Docker image:

```bash
docker build -t pico -f pkklib.Dockerfile .
```

This command creates a Docker image named pico, which will be used to compile your projects.

### 3. Create Your First Application

Copy the template folder located inside the pkklib repository to your working directory to start a new project:

```bash
cd ..
cp -r pkklib/template ./my-first-project
```

### 4. Open the Project in Visual Studio Code

Open the new project folder in Visual Studio Code:

```bash
code .
```

Then run the command Remote-Containers: **Reopen in Container** via the command palette (⇧⌘P on macOS or Ctrl+Shift+P on Windows/Linux).

### 5. Compile the Project

Once inside the container, open a terminal (Terminal → New Terminal) and compile the project using:

```bash
cmake . && make
```

You could also run the compilation by running the following command in your projet folder

```bash
docker run --rm -v "${PWD}:/src/" -v "${PWD}/../pkklib:/src/pkklib" -it pico
cmake . && make
```

The template project will be compiled and the corresponding binaries will be generated.

⸻

Your development environment is now ready to use pkklib!

## Update pkklib

If updates are available on the pkklib GitHub repository, you can update your local setup as follows:

### 1. Pull the Latest Changes

Inside the PiKoKalc directory:

```bash
cd PiKoKalc
git pull
```

### 2. Rebuild the Docker Image

Rebuild the Docker image using the latest base image and source code:

```bash
docker build --pull --no-cache -t pico -f pkklib.Dockerfile .
```

### 3. Update Your Project’s CMakeLists.txt

In your project (e.g., my-first-project), open the CMakeLists.txt file and replace the section between the following lines:

```bash
---- BEGIN PKKLIB CONFIG ----
...
---- END PKKLIB CONFIG ----
```

with the updated content from the template/CMakeLists.txt file in the pkklib repository.

This ensures your project benefits from the latest pkklib configuration and features.

