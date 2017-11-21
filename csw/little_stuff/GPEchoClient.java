
package csw.little_stuff;

import java.io.*;
import java.lang.Thread;
import java.net.*;

public class GPEchoClient
{

    public static void main (String[] args) 
    {
        Socket           echoSocket = null;
        PrintWriter      out = null;
        BufferedReader   in = null;

        try {
            echoSocket = new Socket ("dover", 8888);
            out = new PrintWriter (echoSocket.getOutputStream(), true);
            in = new BufferedReader 
                (new InputStreamReader(echoSocket.getInputStream()));
        }
        catch (UnknownHostException e) {
            System.err.println ("Don't know about host dover.");
            System.exit (1);
        }
        catch (IOException e)
        {
            System.err.println ("Could not get i/o for connection to dover");
            System.exit(1);
        }

        BufferedReader stdIn = 
            new BufferedReader (new InputStreamReader(System.in));
        String userInput;

        while (true) {

            try {
                userInput = stdIn.readLine();
                if (userInput == null) {
                    break;
                }
            }
            catch (IOException e)
            {
                break;
            }

            if (userInput.equals("quit")) {
                break;
            }

            try {
                out.println (userInput);
                System.out.println (in.readLine());
            }
            catch (IOException e)
            {
                break;
            }

            if (userInput.equals("kill")) {
                break;
            }

        }

        try {
            out.close();
            in.close();
            stdIn.close();
            echoSocket.close();
        }
        catch (IOException e)
        {
            System.err.println ("Problem closing client stuff.");
        }

    }

}
