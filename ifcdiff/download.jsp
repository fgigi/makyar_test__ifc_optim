<%@page import="java.io.*"%>
<%@page import="java.util.*"%>
<%@page import="javax.servlet.*"%>
<%@page import="javax.servlet.http.*"%>
<%
		String serverName=request.getServerName();
		String path=request.getRealPath(serverName);
		path=path.substring(0,path.lastIndexOf("\\"));
		path=path+"\\upload\\";
		String filename = request.getParameter("name");
		response.setContentType("APPLICATION/OCTET-STREAM");
		response.setHeader("Content-Disposition","attachment; filename=\"" + filename + "\"");

		// ��ָ���ļ�������Ϣ
		java.io.FileInputStream fileInputStream =
		new java.io.FileInputStream(path + filename);

		// д������Ϣ
		int i;
		while ((i=fileInputStream.read()) != -1) {
		out.write(i);
		}
		fileInputStream.close();
		out.close();
%>