# Ollama C++ Client  
*A simple terminal-based client to interact with local Ollama models for LLM inference*  

## 📦 Features  
- Lists available Ollama models with human-readable sizes  
- Interactive model selection  
- Sends prompts to Ollama API and displays responses  
- Built with CMake, libcurl, and nlohmann/json   

---

## 🛠️ Requirements  
Before building, install dependencies:  
- curl (from [source](https://github.com/curl/curl))
- json (from [source](https://github.com/nlohmann/json))
- libcurl
```bash
apt-get install libnghttp2-dev libidn2-dev libpsl-dev
```  
Ensure [Ollama](https://ollama.ai/) is installed and running (`ollama serve`).  

---

## 🧱 Build Instructions  
### Clone this repo and move into it
```bash
git clone https://github.com/voltvirtuoso/Ollama-Cpp-Query.git
cd Ollama-Cpp-Query
```

### Option 1: CMake (Recommended)  
```bash
mkdir -p build && cd build
cmake ..      # Generates Makefiles using CMakeLists.txt 
make          # Compiles the project
```  

### Option 2: Makefile  
Run directly without CMake:  
```bash
make          # Uses Makefile to compile ollama_cpp
```  

---

## ▶️ Running the Client  
After building, execute the binary:  
```bash
./ollama_cpp  # Starts the interactive query tool
```  

### Example Workflow  
1. Select a model (default is any mid weight model available).  
2. Type a prompt (e.g., `"Explain quantum computing"`).  
3. Type `"q"` to quit.  

---

## 📁 File Structure  
- `CMakeLists.txt`: CMake configuration for cross-platform builds   
- `Makefile`: Simplified build wrapper (uses `-lcurl` linking)  
- `ollama_cpp.cpp`: Core logic for Ollama interaction  

---

## 🔐 License  
Creative Commons CC0

---

## ⚠️ Notes  
- If no models are listed, pull one via Ollama CLI:  
  ```bash
  ollama pull llama2
  ```  
- For custom API endpoints, modify URLs in `fetchJsonFromUrl()` in `ollama_cpp.cpp`.  

---
