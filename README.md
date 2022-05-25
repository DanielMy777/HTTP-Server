# HTTP-Server

Made entirely in C++, this simple HTTP server can support up to 60 simultaneous connections,
recieve requests, parse messages, and provide responses.

## ⚡️ Quick Deployment

> You're going to need some sample resources, use the ones provided in 'Resources' or create your own.

1. Copy your resources into `C:/temp` - This is the root folder of the server.
3. Create a new `Visual Studio` solution and add the source files.
5. Build & run the solution and you're good to go.

## ⚙️ Supported HTTP Methods
* GET - Get a resource from the root directory of the server.
* POST - The server will print the body of the post request onto the console.
* HEAD - Just like GET, but the response will not have a body (headers only).
* OPTIONS - The server will list the available methods in a special header 'Allow'.
* TRACE - The response body will contain the full HTTP request by the client.
* PUT - Add / Update a file in the root directory.
* DELETE - Delete a file from the root directory.

## 📖 Additional Info

You can find additional information and instructions in the supplied `API.docx` file.

## ⭐️ Contributors

* Daniel Malky

> Feel free to add any contribution, it will be blessed.
