
package udpstreamingreceiver;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author User
 */
public class UDPStreamingReceiver extends Thread {
    private static final Logger logger = Logger.getLogger(UDPStreamingReceiver.class.getName());
    private static final int UPD_BUFFER_SIZE = 65536;
    private static final int MAX_BUFFER_SIZE = 2;
    private static final double ACCEPT_THRESHOLD = 0.7;
    private final Queue<ByteBuffer> dataQueue;
    private final List<BufferStruct> dataBuffer;
    private final DatagramSocket socket;
    private boolean keepRunning = false;
    private int buffCount = 0; 
    private int frameCount = 0;
    
    /**
     * @param args the command line arguments
     * @throws java.net.SocketException
     * @throws java.lang.InterruptedException
     */
    public static void main(String[] args) throws SocketException, InterruptedException {
        
        UDPStreamingReceiver rec = new UDPStreamingReceiver(17788);
        rec.start();
        
        ByteBuffer buff = null;
        while (true) {
            if ((buff = rec.getData()) == null) {
                Thread.sleep(100);
                continue;
            }
            //System.out.println(buff.position() + " - " + buff.limit() + " - " + buff.capacity());
            buff.clear();
        }
        
    }

    public class BufferStruct {
        ByteBuffer data;
        int tag, length, count;
    }
    
    public UDPStreamingReceiver(int bindPort) throws SocketException {   
        logger.log(Level.INFO, "listen port : {0}", bindPort);
        socket = new DatagramSocket(bindPort);
        dataBuffer = new ArrayList<>();
        dataQueue = new LinkedList<>();
        for (int i = 0; i < MAX_BUFFER_SIZE; ++i) {
            dataBuffer.add(new BufferStruct());
            dataBuffer.get(i).data = ByteBuffer.allocate(0);
        }
    }
    
    public ByteBuffer getData () {
        synchronized (dataQueue) {
            return dataQueue.poll();
        }
    }
    
    private void addDataToBuffer(byte[] rawData, int dataLength){
        int packageTag = ByteBuffer.wrap(rawData, 0, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageLength = ByteBuffer.wrap(rawData, 4, 8).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageSize = ByteBuffer.wrap(rawData, 8, 12).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageShift = ByteBuffer.wrap(rawData, 12, 16).order(ByteOrder.LITTLE_ENDIAN).getInt();
        
        //System.out.println(packageTag);
        //System.out.println(packageLength);
        //System.out.println(packageSize);
        //System.out.println(packageShift);
        
        BufferStruct buff = null;
        for (BufferStruct b : dataBuffer)
            if (b.tag == packageTag)
                buff = b;
        
        //for (int i = 0; i < MAX_BUFFER_SIZE; ++i)
            //if (dataBuffer.get(i).tag == packageTag) {
                //buff = dataBuffer.get(i);
                //if (i != buffCount)
                    //System.out.println(i + "!=" + buffCount);
            //}
        
        
        if (buff == null) {
            buffCount = ++buffCount % MAX_BUFFER_SIZE;
            buff = dataBuffer.get(buffCount);
            
            if (buff.count < buff.length) {
                if ((double)buff.count / buff.length > ACCEPT_THRESHOLD) {
                    synchronized (dataQueue) {
                        dataQueue.add(buff.data);
                    }
                    frameCount++;
                    logger.log(Level.INFO, "accept package ({1}/{2}) : {0}", 
                                new Object[]{buff.tag, buff.count, buff.length});
                }
                else {
                    logger.log(Level.WARNING, "loss package ({1}/{2}) : {0}", 
                                new Object[]{buff.tag, buff.count, buff.length});
                }   
            }
            
            buff.count = 0;
            buff.tag = packageTag;
            buff.length = packageLength;
            
            byte[] tmp = buff.data.array();
            int limit = tmp.length > packageSize ? packageSize : tmp.length;
            
            buff.data = ByteBuffer.allocate(packageSize);
            buff.data.put(tmp, 0, limit);
        }
        
        //logger.log(Level.INFO, "update package : {0}", packageTag);
        buff.count++;
        
        buff.data.position(packageShift);
        buff.data.put(rawData, 16, dataLength - 16);
        buff.data.rewind();
        
        if (buff.count == buff.length) {
            //logger.log(Level.INFO, "package receive complete : {0}", packageTag);
            synchronized (dataQueue) {
                dataQueue.add(buff.data);
            }
            frameCount++;
        }
    }
    
    @Override
    public void run() {
        byte[] buf = new byte[UPD_BUFFER_SIZE];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        int frame = 0, recv = 0;
        double sec = 0;
        long t = 0;
        
        keepRunning = true;
        
        while (keepRunning) {
            t = System.nanoTime();
            try {
                t = System.nanoTime();
                socket.receive(packet);
            } catch (IOException ex) {
                logger.log(Level.WARNING, ex.toString());
                continue;
            }
            sec += (System.nanoTime() - t) / 1000.0 / 1000.0 / 1000.0;
            recv += packet.getLength();
            
            if (sec > 1) {
                logger.log(Level.INFO, "Network speed : {0}KB/s  Frame rate : {1}", 
                        new Object[]{ (recv / sec / 1024.0), (frameCount - frame) / sec});
                recv = 0; frame = frameCount; sec = 0;
            }
            
            addDataToBuffer(packet.getData(), packet.getLength());
            packet.setLength(UPD_BUFFER_SIZE);
        }
    }
    
}
