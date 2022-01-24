# Basic File Transfer Protocol (BFT)

## Format

```text
 0      7 8     15 16    23 24    31
+--------+--------+--------+--------+
|                                   |
|             Checksum              |
+--------+--------+--------+--------+
|                 |                 |
|      Flags      |    data size    |
+--------+--------+--------+--------+
|
|              data ...
+---------------- ...
```

### Flags

| Abbr. | Name            | Sender          | Desc.                                                            |
|-------|-----------------|-----------------|------------------------------------------------------------------|
| SQN   | Sequence number | Client / Server |                                                                  |
| ACK   | Acknowledge     | Server          |                                                                  |
| FIN   | Finish          | Client          | Transmission of file is complete                                 |
| ABR   | Abort           | Client / Server | Upload should be aborted.                                        |
| SYN   | Synchronize     | Client          | Initiates a file upload. The payload will determine the filename |
| AGN   | Again           | Server          | Request for retransmission                                       |
