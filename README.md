# Simulation Core

This is an application that has been developed as an multiuser server based simulation core for browser games, but has evolved to 
an server based application core system.

Do not hesitate to ask for more information. 

## First of all

I came to the conclusion that I will never be able to make any use of this code, anyways I believe it contains many good ideas.
The code is somewhat old and has run through many iterations until it has come to the state it is now.

One of the earlies file headers I have seen are from January 2011. So this code base has been created before C++-11 standard was widely used.
Such sources are probably even some years older.

So think of it if you come along some weird code fragments, e.g these file-access classes that could now be changed to C++ standard implementations.

Documentation is of course not existing. I am still available to fix this is someone has interest in using the software.

##  Components (more or less complete)

  * Simulation Core
    * Time descrete updates
    * Event based updates
  * Web-Server
  * WebSocket server
  * WebSocket client code.
  * PostgreSQL database access

## Support tool

The only existing support tool is a code generator for UML tools that can be used to maintain the simulation.
The generator creates tons of code for simulation objects. So even it is possible to write the simulation manually 
I would not suggest doing so.

On the other hand you may need that freedom.

