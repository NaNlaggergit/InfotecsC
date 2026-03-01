CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC
LDFLAGS = -shared
APPFLAGS = -std=c++17 -Wall -Wextra

LIB_TARGET = liblibrary.so
APP_TARGET = library_app

LIB_SRC = src/library.cpp
LIB_OBJ = $(LIB_SRC:.cpp=.o)

APP_SRC = app/main.cpp

INCLUDE = -Iinclude

all: $(LIB_TARGET) $(APP_TARGET)

$(LIB_TARGET): $(LIB_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(APP_TARGET): $(APP_SRC)
	$(CXX) $(APPFLAGS) $(APP_SRC) -L. -llibrary -Wl,-rpath=. $(INCLUDE) -o $@

clean:
	rm -f $(LIB_OBJ) $(LIB_TARGET) $(APP_TARGET)