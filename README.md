# RN

### Load project in CLion
- Open `rn` directory in CLion
- Right click on top-level `CMakeLists.txt` -> `Load CMake Project`
- Now you can select the runconfigs `http_server` and `snatch` in the top right corner

### UML Diagrams

```plantuml
frame "cd Rechnernetze"{
package "connection_base" {
    class Connection{
        - file_descriptor: int
        + <<Create>> (host: string)
        + <<Create>> (address: Addres, file_descriptor: int)
        + receive_string(): string
        + receive_bytes(): char[]
        + send(message: string)
        + send_slow(message: string)
    }
}

package "http_server"{

class ConnectionListener {
  - file_descriptor: int
  - port: int
  + <<Create>> (port: int, backlog: int)
  + accept_next_connection() : Connection
  + shutdown()
}

ConnectionListener::accept_next_connection ..> Connection : <<creates>>

}
}
