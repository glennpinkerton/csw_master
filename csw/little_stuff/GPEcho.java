
package csw.little_stuff;

import java.awt.event.*;
import java.io.*;
import java.lang.Thread;
import java.net.*;
import javax.swing.Timer;

public class GPEcho
{
    ServerSocket  serverSocket;
    boolean       serverTimedOut;
	boolean       serverKillRequested;
	int           numActiveThreads;

    public static void main (String[] args) 
    {
        GPEcho  instance = new GPEcho ();
        instance.startup ();
    }

    private void startup ()
    {
        serverTimedOut = false;
		serverKillRequested = false;
        try {
            serverSocket = new ServerSocket (8888);
        }
        catch (IOException e)
        {
            System.err.println ("Could not listen on port 8888");
            System.exit(1);
        }

    /*
     * Put in a timer to get out of loop if not active for a long time.
     * If there is no client activity for about 20 minutes (1200 seconds)
	 * the server kills itself.
     */
        ExitServerTask exitServer = new ExitServerTask ();
        KillServerTask killServer = new KillServerTask ();

        Timer exitTimer = new Timer (1200000, exitServer);
        exitTimer.start ();

        Timer killTimer = new Timer (1000, killServer);
        killTimer.start ();

        Socket clientSocket = null;
        while (true) {
            try {
                clientSocket = serverSocket.accept ();
            }
            catch (IOException e)
            {
                if (!serverTimedOut) {
                    System.err.println ("Server accept failed");
                }
                System.exit(1);
            }
            GPEchoThread echoThread = new GPEchoThread (clientSocket);
            echoThread.start ();
            exitTimer.restart();
        }

    }

    private class ExitServerTask implements ActionListener
    {
        public void actionPerformed (ActionEvent e)
        {
            try {
                serverSocket.close ();
            }
            catch (IOException ex) {
                System.err.println ("Error closing server socket.");
            }
            System.out.println ("Closing echo server after timeout.");
            System.exit(1);
        }
    }

    private class KillServerTask implements ActionListener
    {
        public void actionPerformed (ActionEvent e)
        {
			if (serverKillRequested == false) {
				return;
			}

			int nthread = Thread.activeCount ();
			System.out.println ("nthread = " + nthread);

            try {
                serverSocket.close ();
            }
            catch (IOException ex) {
                System.err.println ("Error closing server socket.");
            }
            System.out.println ("Closing echo server after kill request.");
            System.exit(1);
        }
    }

    private class GPEchoThread extends Thread
    {
        private Socket             clientSocket;
        private PrintWriter        out;
        private BufferedReader     in;

        public GPEchoThread (Socket cSock)
        {
            try {
                clientSocket = cSock;
                out = new PrintWriter (clientSocket.getOutputStream(), true);
                in = new BufferedReader 
                    (new InputStreamReader (clientSocket.getInputStream()));
            }
            catch (IOException e) {
                System.err.println ("Echo Thread Failed.");
                System.exit (1);
            }
        }
    
        public void run ()
        {
            String   inLine, outLine;
    
            while (true) {
                try {
                    inLine = in.readLine ();
                }
                catch (IOException eIO) {
                    break;
                }
                if (inLine == null) {
                    break;
                }
                inLine = inLine.trim();
                if (inLine.equals("kill")) {
					serverKillRequested = true;
                    break;
                }
                if (inLine.equals("")) {
                    break;
                }
                outLine = "Echo: " + inLine;
    			out.println (outLine);
            }
    
            try {
                out.close();
                in.close();
                clientSocket.close();
            }
            catch (IOException eIO) {
                System.err.println ("Could not close the socket streams.");
            }
        }
    }
    
}
