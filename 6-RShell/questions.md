1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The EOF character is used to signal when a command's output is fully received from the server. We can ensure that the entire message is transmitted by looping through the message until the EOF character is found. One technique that can be used to handle partial reads would be sending the length of the message before the actual data, so we know how large of a message to expect. The main method I used, however, was just looping until the EOF character is found which will signal to the client that the transmission is complete.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol can detect the beginning and end of a command by using some sort of delimiter/special character, such as "\0" or EOF to mark the beginning and end of a message. Additionally, it can send the length of the actual command before sending the data so that the shell protocol knows how long to expect the command to be. If this isn't handled correctly, this can result in major challenges such as being stuck in recv() indefinitely if the networked shell protocol doesn't know when to stop reading the command. Additionally, if the command is not read properly it could end up being split into different messages, therefore not executing what the command was originally intended to do.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols refer to maintaining state information between requests, while stateless protocols don't maintain state between requests. Stateful requires the server to store the current client state, while stateless doesn't store a client state on the server. Stateful protocols are much less scalable than statless protocols, since as the protocols scale, stateful will require too much memory. Stateful protocols are much more vulnerable to server crashes, as if the server crashes the state may be lost, while stateless protocols don't have to worry about this.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is seen as unreliable because it risks packets being lost, the packets being out of order, and duplicate packet detection. Despite this, UDP may be useful for things like requiring a low overhead, since UDP doesn't require a connection setup. Additionally, UDP has much lower latency than TCP. UDP also has support for sending data to multiple recipients at once, which can be very efficient. Because UDP has these features that TCP lacks, it can be useful in some scenarios, including multiplayer video games and streaming.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The socket API allows for network communications. This allowed for functions such as socket(), bind(), listen(), accept(), connect(), send()/recv() and close(), all of which helped establish a network communication between the server and client.