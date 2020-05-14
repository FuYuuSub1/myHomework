import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;

public final class webserver {
    public static void main(String argv[]) throws Exception{

        int port = 8080;

        //create server socket
        ServerSocket server_socket = new ServerSocket(port);
        System.out.println("Port number: "+server_socket.getLocalPort());

        //Create threads and run http request, the server will not close because it assume to operate 7/24
        while(true){
            Socket request_socket = server_socket.accept();
            HttpRequest request = new HttpRequest(request_socket);

            Thread thread = new Thread(request);
            thread.start();
        }
    }
}

final class HttpRequest implements Runnable{

    final static String crlf = "\r\n";
    Socket soc;

    //constructor
    public HttpRequest(Socket soc) throws Exception{
        this.soc = soc;
    }

    //run method for multithreaded purpose
    public void run(){
        try{
            Process_Request();
        } catch (Exception e){
            System.out.println(e);
        }
    }

    private void Process_Request() throws Exception{

        //Create input stream and output stream
        InputStream input_stream = soc.getInputStream();
        DataOutputStream output_stream = new DataOutputStream(soc.getOutputStream());

        //Read the HTTP request message to buffered reader
        BufferedReader buff_reader = new BufferedReader(new InputStreamReader(input_stream));
        String request_sentence = buff_reader.readLine();

        //Log file name
        String log_file = "log.txt";

        //String to be stored to the log file for every http request
        String log_string = "";

        //Retrieve the access time (current time) and convert it to String,append to the log string
        String ip = (((InetSocketAddress) soc.getRemoteSocketAddress()).getAddress()).toString().replace("/","");
        SimpleDateFormat formatter= new SimpleDateFormat("yyyy-MM-dd 'at' HH:mm:ss z");
        Date date = new Date(System.currentTimeMillis());
        log_string = log_string + "IP: " + ip + " Access Time: " + formatter.format(date);


        //Tokenize the request message in order to retrieve required information
        StringTokenizer tokens = new StringTokenizer(request_sentence);

        //If HTTP request method is not GET or HEAD, it will still do the work because of logging purpose, but response with 400 bad request
        if(tokens.nextToken().equals("GET") || tokens.nextToken().equals("HEAD")){

            //Retrieve the file name and append to the log string
            String file_name = tokens.nextToken();
            file_name = "." + file_name;
            log_string = log_string + " File name: " + file_name;

            //check whether the file exists
            FileInputStream fi_stream = null;
            boolean file_exists = true;
            try{
                fi_stream = new FileInputStream(file_name);
            }catch (FileNotFoundException e){
                file_exists = false;
            }


            String status_sentence = null;
            String content_type_sentence = null;
            String entity_body = null;
            String header_sentence = null;
            
            //If exist, respond with 200 OK message and continue some other handling, otherwise, respond with 404 not found message
            if(file_exists){
                status_sentence = "200 OK";
                content_type_sentence = "Content-type: " + Content_Type(file_name) + crlf ;
                log_string = log_string + " " + status_sentence;
                while((header_sentence = buff_reader.readLine()).length() != 0){
                    
                    //Retrieve If-modified-Since header, compare it to the file's last modified time, response whether is outdated or not outdated
                    if(header_sentence.startsWith("If-Modified-Since:")){
                        String date_string = header_sentence.substring(19);
                        SimpleDateFormat formatter2 = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz");
                        Date d1 = formatter2.parse(date_string);
                        File f2 = new File(file_name);
                        Date d2 = formatter2.parse(formatter2.format(f2.lastModified()));
                        if (d1.compareTo(d2) < 0){
                            log_string = log_string + " outdated";
                        }else{
                            log_string = log_string + " not outdated";
                        }
                    }
                }
                log_string = log_string + "\n";
            } else{
                status_sentence = "404 Not Found";
                content_type_sentence = "Content_type: " + Content_Type(file_name) + crlf ;
                entity_body = "<HTML>" + "<HEAD><TITLE>404 NOT Found</TITLE></HEAD>" +
                "<BODY>404 Not Found</BODY></HTML>";
                log_string = log_string + " " + status_sentence +" \n";
            }

            //output the file to the client
            output_stream.writeBytes(status_sentence);
            output_stream.writeBytes(content_type_sentence);
            output_stream.writeBytes(crlf);
            if(file_exists){
                Send_Bytes(fi_stream, output_stream);
                fi_stream.close();
            }else{
                output_stream.writeBytes(entity_body);
            }

            //close all the things
            output_stream.close();
            buff_reader.close();
            soc.close();
        }else{

            //Retrieve the file name and append to the log string
            String file_name = tokens.nextToken();
            file_name = "." + file_name;
            log_string = log_string + " File name: " + file_name;

            //check whether the file exists
            FileInputStream fi_stream = null;
            boolean file_exists = true;
            try{
                fi_stream = new FileInputStream(file_name);
            }catch (FileNotFoundException e){
                file_exists = false;
            }


            String status_sentence = null;
            String content_type_sentence = null;
            String entity_body = null;
            String header_sentence = null;
            
            //Whatever it exists, it is still bad request because of invalid http request method
            if(file_exists){
                status_sentence = "400 Bad Request";
                content_type_sentence = "Content-type: " + Content_Type(file_name) + crlf ;
                log_string = log_string + " " + status_sentence;
                while((header_sentence = buff_reader.readLine()).length() != 0){
                    
                    //Retrieve If-modified-Since header, compare it to the file's last modified time, response whether is outdated or not outdated
                    if(header_sentence.startsWith("If-Modified-Since:")){
                        String date_string = header_sentence.substring(19);
                        SimpleDateFormat formatter2 = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss zzz");
                        Date d1 = formatter2.parse(date_string);
                        File f2 = new File(file_name);
                        Date d2 = formatter2.parse(formatter2.format(f2.lastModified()));
                        if (d1.compareTo(d2) < 0){
                            log_string = log_string + " outdated";
                        }else{
                            log_string = log_string + " not outdated";
                        }
                    }
                }
                log_string = log_string + "\n";
            } else{
                status_sentence = "400 Bad Request";
                content_type_sentence = "Content_type: " + Content_Type(file_name) + crlf ;
                entity_body = "<HTML>" + "<HEAD><TITLE>404 NOT Found</TITLE></HEAD>" +
                "<BODY>404 Not Found</BODY></HTML>";
                log_string = log_string + " " + status_sentence +" \n";
            }
            output_stream.close();
            buff_reader.close();
            soc.close();
        }

        //print the log string for testing purpose
        System.out.print(log_string);
        
        //append the whole log string to the log file
        try{
            BufferedWriter out_log = new BufferedWriter(new FileWriter(log_file, true));
            out_log.write(log_string);
            out_log.close();
        }catch (IOException e){
            System.out.println("Exception Occurred" + e);
        }
        
    }

    //send the byte
    private static void Send_Bytes(FileInputStream file_input_stream, OutputStream output_stream) throws Exception{
        byte[] buffer = new byte[1024];
        int bytes = 0;
        while((bytes = file_input_stream.read(buffer)) != -1){
            output_stream.write(buffer, 0, bytes);
        }
    }

    //produce the content type
    private static String Content_Type(String file_name){
        if(file_name.toLowerCase().endsWith(".htm") || file_name.toLowerCase().endsWith(".html")){
            return "text/html";
        }else if(file_name.toLowerCase().endsWith(".gif")){
            return "image/gif";
        }else if(file_name.toLowerCase().endsWith(".jpg")){
            return "image/jpeg";
        }else if(file_name.toLowerCase().endsWith(".txt")){
            return "text/txt";
        }else{
            return "application/octet-stream";
        }
    }
}