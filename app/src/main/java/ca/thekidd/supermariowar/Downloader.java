package ca.thekidd.supermariowar;


import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Downloader extends Activity {

    private final String TAG = "Downloader";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if(getExternalFilesDir(null).listFiles(new FileFilter() {
                    @Override
                    public boolean accept(File pathname) {
                        return pathname.isDirectory();
                    }
                }).length != 0) {
            startActivity(new Intent(this, MainActivity.class));
            finish();
            return;
        }

        AlertDialog.Builder adb = new AlertDialog.Builder(this);
        adb.setTitle("Additional Files Required");
        adb.setMessage("To run Super Mario War an additional 12.5MB of files need to be downloaded.");
        adb.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                downloadFiles();
            } });
        adb.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                finish();
            } });
        adb.show();
    }

    private void downloadFiles() {
        final ProgressDialog progressDialog = new ProgressDialog(this);
        progressDialog.setMessage("Downloading additional files...");
        progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
        progressDialog.show();

        String url = "https://github.com/mmatyas/supermariowar/raw/master/data.zip";
        DownloadFileAsync download = new DownloadFileAsync(getExternalFilesDir(null).getAbsolutePath() + "/data.zip", this, new DownloadFileAsync.ProgressUpdate() {
            @Override
            public void downloadProgress(int progress) {
                progressDialog.setProgress(progress);
            }
        }, new DownloadFileAsync.PostDownload(){
            @Override
            public void downloadDone(File file) {
                Log.i(TAG, "file download completed");

                Decompress unzip = new Decompress(Downloader.this, file, getExternalFilesDir(null).getAbsolutePath() + "/supermariowar/");
                unzip.unzip();
                file.delete();

                Log.i(TAG, "file unzip completed");
                progressDialog.dismiss();
                startActivity(new Intent(Downloader.this, MainActivity.class));
                finish();
            }
        });
        download.execute(url);
    }

    public static class DownloadFileAsync extends AsyncTask<String, String, String> {

        private static final String TAG ="DOWNLOADFILE";

        public static final int DIALOG_DOWNLOAD_PROGRESS = 0;
        private ProgressUpdate updateCallback;
        private PostDownload callback;
        private Context context;
        private FileDescriptor fd;
        private File file;
        private String downloadLocation;

        public DownloadFileAsync(String downloadLocation, Context context, ProgressUpdate updateCallback, PostDownload callback){
            this.context = context;
            this.updateCallback = updateCallback;
            this.callback = callback;
            this.downloadLocation = downloadLocation;
        }
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected String doInBackground(String... aurl) {
            int count;

            try {
                URL url = new URL(aurl[0]);
                URLConnection connection = url.openConnection();
                connection.connect();

                int lenghtOfFile = connection.getContentLength();
                Log.d(TAG, "Length of the file: " + lenghtOfFile);

                InputStream input = new BufferedInputStream(url.openStream());
                file = new File(downloadLocation);
                FileOutputStream output = new FileOutputStream(file); //context.openFileOutput("content.zip", Context.MODE_PRIVATE);
                Log.d(TAG, "file saved at " + file.getAbsolutePath());
                fd = output.getFD();

                byte data[] = new byte[1024];
                long total = 0;
                while ((count = input.read(data)) != -1) {
                    total += count;
                    publishProgress(""+(int)((total*100)/lenghtOfFile));
                    output.write(data, 0, count);
                }

                output.flush();
                output.close();
                input.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;

        }
        protected void onProgressUpdate(String... progress) {
            Log.d(TAG,progress[0]);
            if(updateCallback != null) updateCallback.downloadProgress(Integer.parseInt(progress[0]));
        }

        @Override
        protected void onPostExecute(String unused) {
            if(callback != null) callback.downloadDone(file);
        }

        public static interface PostDownload{
            void downloadDone(File fd);
        }

        public static interface ProgressUpdate{
            void downloadProgress(int progress);
        }
    }

    public static class Decompress {
        private File _zipFile;
        private InputStream _zipFileStream;
        private Context context;
        private String rootLocation;
        private static final String TAG = "UNZIPUTIL";

        public Decompress(Context context, File zipFile, String rootLocation) {
            this.rootLocation = rootLocation;
            _zipFile = zipFile;
            this.context = context;

            _dirChecker("");
        }

        public void unzip() {
            try  {
                Log.i(TAG, "Starting to unzip");
                InputStream fin = _zipFileStream;
                if(fin == null) {
                    fin = new FileInputStream(_zipFile);
                }
                ZipInputStream zin = new ZipInputStream(fin);
                ZipEntry ze = null;
                while ((ze = zin.getNextEntry()) != null) {
                    Log.v(TAG, "Unzipping " + ze.getName());

                    if(ze.isDirectory()) {
                        _dirChecker(rootLocation + "/" + ze.getName());
                    } else {
                        FileOutputStream fout = new FileOutputStream(new File(rootLocation, ze.getName()));
                        ByteArrayOutputStream baos = new ByteArrayOutputStream();
                        byte[] buffer = new byte[1024];
                        int count;

                        // reading and writing
                        while((count = zin.read(buffer)) != -1)
                        {
                            baos.write(buffer, 0, count);
                            byte[] bytes = baos.toByteArray();
                            fout.write(bytes);
                            baos.reset();
                        }

                        fout.close();
                        zin.closeEntry();
                    }

                }
                zin.close();
                Log.i(TAG, "Finished unzip");
            } catch(Exception e) {
                Log.e(TAG, "Unzip Error", e);
            }

        }

        private void _dirChecker(String dir) {
            File f = new File(dir);
            Log.i(TAG, "creating dir " + dir);

            if(dir.length() >= 0 && !f.isDirectory() ) {
                f.mkdirs();
            }
        }
    }
}
